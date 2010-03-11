#include "AboutFrame.h"

AboutFrame::AboutFrame( wxWindow* parent )
		: AboutFramePrototype( parent )
{
	SetTitle(LangGet("AboutFrame_title"));
	titleLabel->SetLabel(LangGet("AboutFrame_titleLabel_value"));
	internalTitleLabel->SetLabel(LangGet("AboutFrame_internalTitleLabel_value"));
	versionLabel->SetLabel(LangGet("AboutFrame_versionLabel_value", wxT(GGUI_VERSION_STRING)));
	authorLabel->SetLabel(LangGet("AboutFrame_authorLabel_value"));
	licenseLabel->SetLabel(LangGet("AboutFrame_licenseLabel_value"));
	sizer->Fit(this);
}
