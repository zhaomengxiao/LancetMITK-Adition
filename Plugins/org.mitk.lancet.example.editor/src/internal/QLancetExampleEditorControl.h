#ifndef QLancetExampleEditorControl_H
#define QLancetExampleEditorControl_H

#include <QWidget>

namespace Ui
{
	class QLancetExampleEditorFrom;
} // namespace Ui

class QLancetExampleEditorControl : public QWidget
{
	Q_OBJECT
public:
	explicit QLancetExampleEditorControl(QWidget* = nullptr);
	virtual ~QLancetExampleEditorControl();
private:
	Ui::QLancetExampleEditorFrom* ui;
};

#endif // !QLancetExampleEditorControl_H