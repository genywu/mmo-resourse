INCLUDES = -I$(top_srcdir)/inc -I$(top_srcdir)/dependencies/iniFile $(CEGUI_CFLAGS) $(WX_CPPFLAGS) $(CLEDITOR_CFLAGS)

bin_PROGRAMS = CELayoutEditor

CELayoutEditor_SOURCES = ../dependencies/iniFile/iniFile.cpp \
						CELayoutEditor.cpp \
						CommandAddWindow.cpp \
						CopiedSelection.cpp \
						DialogAbout.cpp \
						DialogAddWindow.cpp \
						DialogGrid.cpp \
						DialogMain.cpp \
						EditorCanvas.cpp \
						EditorDocument.cpp \
						EditorFrame.cpp \
						EditorView.cpp \
						ExceptionManager.cpp \
						Options.cpp \
						Selection.cpp \
						SelectionMover.cpp \
						WindowBox.cpp \
						WindowContext.cpp

# these are used on windows only at the moment
EXTRA_DIST = pch.cpp \
			SimpleSymbolEngine.cpp

CELayoutEditor_LDADD = $(CEGUI_LIBS) $(WX_LIBS)
