#include <internal/lancetNamingFormats.h>
#include <lancetIScxmlStateMachineState.h>

int main()
{
	std::cout << lancet::plugin::ToMitkId("org_mitk_lancet").toStdString() << std::endl;
	return 1;
}