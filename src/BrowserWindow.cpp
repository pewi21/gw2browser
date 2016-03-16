/* \file       BrowserWindow.cpp
*  \brief      Contains definition of the browser window.
*  \author     Rhoot
*/

/*
Copyright (C) 2014-2016 Khral Steelforge <https://github.com/kytulendu>
Copyright (C) 2012 Rhoot <https://github.com/rhoot>

This file is part of Gw2Browser.

Gw2Browser is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "stdafx.h"

#include <wx/aui/aui.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/stdpaths.h>

#include "Imported/crc.h"

#include "CategoryTree.h"
#include "Exporter.h"
#include "FileReader.h"
#include "ProgressStatusBar.h"
#include "PreviewPanel.h"
#include "PreviewGLCanvas.h"
#include "version.h"

#include "Tasks/ReadIndexTask.h"
#include "Tasks/ScanDatTask.h"
#include "Tasks/WriteIndexTask.h"

#include "BrowserWindow.h"

namespace gw2b {

	namespace {
		enum {
			ID_ShowFileList = wxID_HIGHEST + 1,
			ID_ShowLog,							// Show log panel
			ID_ClearLog,
			//ID_ResetLayout,
			//ID_SetBackgroundColor,
			//ID_ShowGrid,		// Show grid on PreviewGLCanvas
			//ID_ShowMask,		// Apply white texture with black background, no shader
			//ID_SetCanvasSize,	// Set PreviewGLCanvas size
		};
	};

	BrowserWindow::BrowserWindow( const wxString& p_title, const wxSize p_size )
		: wxFrame( nullptr, wxID_ANY, p_title, wxDefaultPosition, p_size )
		, m_index( std::make_shared<DatIndex>( ) )
		, m_progress( nullptr )
		, m_currentTask( nullptr )
		, m_catTree( nullptr )
		, m_previewPanel( nullptr )
		, m_previewGLCanvas( nullptr ) {
		// Notify wxAUI which frame to use
		m_uiManager.SetManagedWindow( this );

		auto menuBar = new wxMenuBar;

		// File menu
		auto fileMenu = new wxMenu;
		wxAcceleratorEntry accel( wxACCEL_CTRL, 'O' );
		fileMenu->Append( wxID_OPEN, wxT( "&Open" ), wxT( "Open a file for browsing" ) )->SetAccel( &accel );
		fileMenu->AppendSeparator( );
		fileMenu->Append( wxID_EXIT, wxT( "E&xit\tAlt+F4" ) );
		// View menu
		auto viewMenu = new wxMenu;
		viewMenu->AppendCheckItem( ID_ShowFileList, wxT( "&Show File List" ) );
		viewMenu->AppendCheckItem( ID_ShowLog, wxT( "&Show Log" ) );
		//viewMenu->Append( ID_ResetLayout, wxT( "&Reset Layout" ) );
		viewMenu->AppendSeparator( );
		viewMenu->Append( ID_ClearLog, wxT( "&Clear Log" ) );
		//viewMenu->AppendSeparator( );
		//viewMenu->Append( ID_SetBackgroundColor, wxT( "&Set Background color" ) );
		//viewMenu->AppendCheckItem( ID_ShowGrid, wxT( "&Show Grid" ) );
		//viewMenu->AppendSeparator( );
		//viewMenu->Append( ID_SetCanvasSize, wxT( "&Set GLCanvas Size" ) );
		// Help menu
		auto helpMenu = new wxMenu( );
		helpMenu->Append( wxID_ABOUT, wxT( "&About Gw2Browser" ) );

		// Attach menu
		menuBar->Append( fileMenu, wxT( "&File" ) );
		menuBar->Append( viewMenu, wxT( "&View" ) );
		//menuBar->Append( settingsMenu, wxT( "&Settings" ) );
		menuBar->Append( helpMenu, wxT( "&Help" ) );
		this->SetMenuBar( menuBar );

		// Setup statusbar
		m_progress = new ProgressStatusBar( this );
		this->SetStatusBar( m_progress );

		// Category tree
		m_catTree = new CategoryTree( this );
		m_catTree->setDatIndex( m_index );
		m_catTree->addListener( this );

		// Preview panel
		m_previewPanel = new PreviewPanel( this );

		// Model Viewer
		m_previewGLCanvas = new PreviewGLCanvas( this );

		// Text control use for loging
		m_log = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize( 600, 70 ), wxTE_MULTILINE );
		m_logTarget = wxLog::SetActiveTarget( new wxLogTextCtrl( m_log ) );

		// Add the panes to the manager
		// CategoryTree
		m_uiManager.AddPane( m_catTree, wxAuiPaneInfo( ).Name( wxT( "CategoryTree" ) ).Caption( wxT( "File List" ) ).BestSize( wxSize( 170, 500 ) ).Left( ) );
		// Log window
		m_uiManager.AddPane( m_log, wxAuiPaneInfo( ).Name( wxT( "LogWindow" ) ).Caption( wxT( "Log" ) ).Bottom( ).Layer( 1 ).Position( 1 ).Hide( ) );
		// Main content window
		m_uiManager.AddPane( m_previewPanel, wxAuiPaneInfo( ).Name( wxT( "panel_content" ) ).CenterPane( ).Hide( ) );
		m_uiManager.AddPane( m_previewGLCanvas, wxAuiPaneInfo( ).Name( wxT( "gl_content" ) ).CenterPane( ).Hide( ) );

		// Set default settings
		this->SetDefaults( );

		// Tell the manager to "commit" all the changes just made
		m_uiManager.Update( );

		// Hook up events
		this->Bind( wxEVT_MENU, &BrowserWindow::onOpenEvt, this, wxID_OPEN );
		this->Bind( wxEVT_MENU, &BrowserWindow::onExitEvt, this, wxID_EXIT );
		this->Bind( wxEVT_MENU, &BrowserWindow::onAboutEvt, this, wxID_ABOUT );
		this->Bind( wxEVT_MENU, &BrowserWindow::onTogglePaneEvt, this, ID_ShowFileList );
		this->Bind( wxEVT_MENU, &BrowserWindow::onTogglePaneEvt, this, ID_ShowLog );
		this->Bind( wxEVT_MENU, &BrowserWindow::onClearLogEvt, this, ID_ClearLog );
		this->Bind( wxEVT_AUI_PANE_CLOSE, &BrowserWindow::onPaneCloseEvt, this );
		this->Bind( wxEVT_CLOSE_WINDOW, &BrowserWindow::onCloseEvt, this );
	}

	//============================================================================/

	BrowserWindow::~BrowserWindow( ) {
		deletePointer( m_currentTask );
		deletePointer( m_logTarget );
		// Deinitialize the frame manager
		m_uiManager.UnInit( );
	}

	//============================================================================/

	bool BrowserWindow::performTask( Task* p_task ) {
		Ensure::notNull( p_task );

		// Already have a task running?
		if ( m_currentTask ) {
			if ( m_currentTask->canAbort( ) ) {
				m_currentTask->abort( );
				deletePointer( m_currentTask );
				this->Unbind( wxEVT_IDLE, &BrowserWindow::onPerformTaskEvt, this );
				m_progress->hideProgressBar( );
			} else {
				deletePointer( p_task );
				return false;
			}
		}

		// Initialize succeeded?
		m_currentTask = p_task;
		if ( !m_currentTask->init( ) ) {
			deletePointer( m_currentTask );
			return false;
		}

		this->Bind( wxEVT_IDLE, &BrowserWindow::onPerformTaskEvt, this );
		m_progress->setMaxValue( m_currentTask->maxProgress( ) );
		m_progress->showProgressBar( );
		return true;
	}

	//============================================================================/

	void BrowserWindow::openFile( const wxString& p_path ) {
		// Try to open the file
		if ( !m_datFile.open( p_path ) ) {
			wxMessageBox( wxString::Format( wxT( "Failed to open file: %s" ), p_path ),
				wxMessageBoxCaptionStr, wxOK | wxCENTER | wxICON_ERROR );
			return;
		}
		wxLogMessage( wxT( "Open dat file: %s" ), p_path );
		m_datPath = p_path;

		// Open the index file
		uint64 datTimeStamp = wxFileModificationTime( p_path );
		auto indexFile = this->findDatIndex( );
		auto readIndexTask = new ReadIndexTask( m_index, indexFile.GetFullPath( ), datTimeStamp );

		// Start reading the index
		readIndexTask->addOnCompleteHandler( [this] ( ) { this->onReadIndexComplete( ); } );
		if ( !this->performTask( readIndexTask ) ) {
			this->reIndexDat( );
		}
	}

	//============================================================================/

	void BrowserWindow::viewEntry( const DatIndexEntry& p_entry ) {
		switch ( p_entry.fileType( ) ) {
		case ANFT_Model:
			if ( m_previewGLCanvas->previewFile( m_datFile, p_entry ) ) {
				m_uiManager.GetPane( wxT( "panel_content" ) ).Hide( );
				m_uiManager.GetPane( wxT( "gl_content" ) ).Show( );
			}
			break;
		default:
			if ( m_previewPanel->previewFile( m_datFile, p_entry ) ) {
				m_uiManager.GetPane( wxT( "panel_content" ) ).Show( );
				m_uiManager.GetPane( wxT( "gl_content" ) ).Hide( );
			}
		}
		m_uiManager.Update( );
	}

	//============================================================================/

	wxFileName BrowserWindow::findDatIndex( ) {
		wxStandardPathsBase& stdp = wxStandardPaths::Get( );
		auto configPath = stdp.GetDataDir( );

		auto datPathCrc = ::compute_crc( INITIAL_CRC, m_datPath.char_str( ), m_datPath.Length( ) );
		auto indexFileName = wxString::Format( wxT( "%x.idx" ), datPathCrc );

		return wxFileName( configPath, indexFileName );
	}

	//============================================================================/

	void BrowserWindow::indexDat( ) {
		auto scanTask = new ScanDatTask( m_index, m_datFile );
		scanTask->addOnCompleteHandler( [this] ( ) { this->onScanTaskComplete( ); } );
		this->performTask( scanTask );
	}

	//============================================================================/

	void BrowserWindow::reIndexDat( ) {
		m_index->clear( );
		m_index->setDatTimestamp( wxFileModificationTime( m_datPath ) );
		this->indexDat( );
	}

	//============================================================================/

	void BrowserWindow::onOpenEvt( wxCommandEvent& WXUNUSED( p_event ) ) {
		wxFileDialog dialog( this, wxFileSelectorPromptStr, wxEmptyString, wxT( "Gw2.dat" ),
			wxT( "Guild Wars 2 DAT|*.dat" ), wxFD_OPEN | wxFD_FILE_MUST_EXIST );

		if ( dialog.ShowModal( ) == wxID_OK ) {
			this->openFile( dialog.GetPath( ) );
		}
	}

	//============================================================================/

	void BrowserWindow::onExitEvt( wxCommandEvent& WXUNUSED( p_event ) ) {
		this->Close( true );
	}

	//============================================================================/

	void BrowserWindow::onAboutEvt( wxCommandEvent& WXUNUSED( p_event ) ) {
		wxAboutDialogInfo info;
		InitAboutInfo( info );

		wxAboutBox( info, this );
	}

	//============================================================================/

	void BrowserWindow::onCloseEvt( wxCloseEvent& p_event ) {
		// Drop out if we can't cancel the window closing
		if ( !p_event.CanVeto( ) ) {
			p_event.Skip( );
			return;
		}

		// Cancel current task if possible.
		if ( m_currentTask ) {
			if ( m_currentTask->canAbort( ) ) {
				m_currentTask->abort( );
				deletePointer( m_currentTask );
				this->Unbind( wxEVT_IDLE, &BrowserWindow::onPerformTaskEvt, this );
			} else {
				this->Disable( );
				m_currentTask->addOnCompleteHandler( [this] ( ) { this->tryClose( ); } );
				p_event.Veto( );
				return;
			}
		}

		// Add a write task if the index is dirty
		if ( !m_currentTask && m_index->isDirty( ) ) {
			auto indexPath = this->findDatIndex( );
			if ( !indexPath.DirExists( ) ) {
				indexPath.Mkdir( 511, wxPATH_MKDIR_FULL );
			}

			auto writeTask = new WriteIndexTask( m_index, indexPath.GetFullPath( ) );
			writeTask->addOnCompleteHandler( [this] ( ) { this->onWriteTaskCloseCompleted( ); } );
			if ( this->performTask( writeTask ) ) {
				this->Disable( );
				p_event.Veto( );
				return;
			}
		}

		p_event.Skip( );
	}

	//============================================================================/

	void BrowserWindow::onPerformTaskEvt( wxIdleEvent& p_event ) {
		Ensure::notNull( m_currentTask );
		m_currentTask->perform( );

		if ( !m_currentTask->isDone( ) ) {
			m_progress->update( m_currentTask->currentProgress( ), m_currentTask->text( ) );
			p_event.RequestMore( );
		} else {
			this->Unbind( wxEVT_IDLE, &BrowserWindow::onPerformTaskEvt, this );
			m_progress->SetStatusText( wxEmptyString );
			m_progress->hideProgressBar( );

			auto oldTask = m_currentTask;
			m_currentTask = nullptr;
			oldTask->invokeOnCompleteHandler( );
			deletePointer( oldTask );
		}
	}

	//============================================================================/

	void BrowserWindow::onTogglePaneEvt( wxCommandEvent &event ) {
		// wxAUI Stuff
		if ( GetMenuBar( )->IsChecked( ID_ShowFileList ) ) {
			m_uiManager.GetPane( wxT( "CategoryTree" ) ).Show( );
		} else {
			m_uiManager.GetPane( wxT( "CategoryTree" ) ).Hide( );
		}

		if ( GetMenuBar( )->IsChecked( ID_ShowLog ) ) {
			m_uiManager.GetPane( wxT( "LogWindow" ) ).Show( );
		} else {
			m_uiManager.GetPane( wxT( "LogWindow" ) ).Hide( );
		}

		m_uiManager.Update( );
	}

	//============================================================================/

	void BrowserWindow::onClearLogEvt( wxCommandEvent &event ) {
		m_log->Clear( );
	}

	//============================================================================/

	void BrowserWindow::onPaneCloseEvt( wxAuiManagerEvent &event ) {
		auto evt = event.GetPane( )->window;
		if ( evt == m_uiManager.GetPane( wxT( "CategoryTree" ) ).window ) {
			this->GetMenuBar( )->Check( ID_ShowFileList, false );
		}
		if ( evt == m_uiManager.GetPane( wxT( "LogWindow" ) ).window ) {
			this->GetMenuBar( )->Check( ID_ShowLog, false );
		}

		m_uiManager.Update( );
	}

	//============================================================================/

	void BrowserWindow::onReadIndexComplete( ) {
		// If it failed, it was cleared.
		if ( m_index->datTimestamp( ) == 0 || m_index->numEntries( ) == 0 ) {
			this->reIndexDat( );
			return;
		}

		// Was it complete?
		auto isComplete = ( m_index->highestMftEntry( ) == m_datFile.numFiles( ) );
		if ( !isComplete ) {
			this->indexDat( );
		}
	}

	//============================================================================/

	void BrowserWindow::onScanTaskComplete( ) {
		auto writeTask = new WriteIndexTask( m_index, this->findDatIndex( ).GetFullPath( ) );
		this->performTask( writeTask );
	}

	//============================================================================/

	void BrowserWindow::onWriteTaskCloseCompleted( ) {
		// Forcing this here causes the OnCloseEvt to not try to write the index
		// again. In case it failed the first time, it's likely to fail again and
		// we don't want to get stuck in an infinite loop.
		this->Close( true );
	}

	//============================================================================/

	void BrowserWindow::tryClose( ) {
		this->Close( false );
	}

	//============================================================================/

	void BrowserWindow::onTreeEntryClicked( CategoryTree& p_tree, const DatIndexEntry& p_entry ) {
		wxLogMessage( wxT( "View Entry: %s" ), p_entry.name( ) );
		this->viewEntry( p_entry );
	}

	//============================================================================/

	void BrowserWindow::onTreeCategoryClicked( CategoryTree& p_tree, const DatIndexCategory& p_category ) {
		// TODO
	}

	//============================================================================/

	void BrowserWindow::onTreeCleared( CategoryTree& p_tree ) {
		// TODO
	}

	//============================================================================/

	void BrowserWindow::onTreeExtractFile( CategoryTree& p_tree, bool p_mode ) {
		auto entries = p_tree.getSelectedEntries( );
		Exporter *exporter;

		if ( entries.GetSize( ) ) {
			if ( p_mode ) {
				exporter = new Exporter( entries, m_datFile, Exporter::EM_Converted );
			} else {
				exporter = new Exporter( entries, m_datFile, Exporter::EM_Raw );
			}
			delete exporter;
		}
	}

	//============================================================================/

	void BrowserWindow::InitAboutInfo( wxAboutDialogInfo& info ) {
		info.SetName( APP_TITLE );
		info.SetVersion( wxString::Format(
			" %d.%d.%d.%d\n%s",
			APP_MAJOR_VERSION,
			APP_MINOR_VERSION,
			APP_RELEASE_NUMBER,
			APP_SUBRELEASE_NUMBER,
			APP_SUBRELEASE_NUMBER ? wxT( "Release" ) : wxT( "Development" )
			) );

		info.SetCopyright( wxString::FromAscii(
			"Copyright (C) 2014-2016 Khral Steelforge - https://github.com/kytulendu\n"
			"Copyright (C) 2013 Till034 - https://github.com/Till034\n"
			"Copyright (C) 2012 Rhoot - https://github.com/rhoot\n"
			"\n"
			"Guild Wars 2 (C)2010-2015 ArenaNet, LLC. All rights reserved.\n"
			"Guild Wars, Guild Wars 2, Guild Wars 2: Heart of Thorns, ArenaNet,\n"
			"NCSOFT, the Interlocking NC Logo, and all associated logos and designs\n"
			"are trademarks or registered trademarks of NCSOFT Corporation.\n"
			"All other trademarks are the property of their respective owners.\n"
			) );

		info.SetDescription( wxString::FromAscii(
			"Opens a Guild Wars 2 .dat file and allows the user to browse and extract\n"
			"its content.\n"
			) );

		info.SetWebSite( wxT( "https://github.com/kytulendu/Gw2Browser" ) );

		info.SetLicence( wxString::FromAscii(
			"Gw2Browser is free software: you can redistribute it and/or modify\n"
			"it under the terms of the GNU General Public License as published by\n"
			"the Free Software Foundation, either version 3 of the License,\n"
			"or ( at your option ) any later version.\n"
			"\n"
			"This program is distributed in the hope that it will be useful,\n"
			"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
			"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
			"GNU General Public License for more details.\n"
			"\n"
			"You should have received a copy of the GNU General Public License\n"
			"along with this program.If not, see <http://www.gnu.org/licenses/>.\n"
			"\n"
			"See README.md for code license exceptions."
			) );

		info.AddDeveloper( wxT( "Khral Steelforge" ) );
		info.AddDeveloper( wxT( "Rhoot" ) );
	}

	//============================================================================/

	void BrowserWindow::SetDefaults( ) {
		this->GetMenuBar( )->Check( ID_ShowFileList, true );
		this->GetMenuBar( )->Check( ID_ShowLog, false );
	}

}; // namespace gw2b
