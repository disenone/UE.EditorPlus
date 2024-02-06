// Copyright (c) 2024 - 2024, Disenone (https://github.com/disenone).
// All rights reserved. Licensed under MIT License.

#pragma once

#ifdef EP_USE_TOOLBAR
	#define EP_TOOLS_PATH "/<Hook>ProjectSettings/<ToolBar>EditorPlusTools"
#else
	#define EP_TOOLS_PATH "/<MenuBar>EditorPlusTools"
#endif
