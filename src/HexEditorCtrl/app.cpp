#include "app.h"
#include "main.h"

IMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
	MyFrame* frame = new MyFrame(0L, _("HexEditorCtrl; 3 in 1 HexEditorGUI"));
	frame->Show();
	return true;
}
