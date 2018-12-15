#include "progressbar.h"

ProgressBar::ProgressBar(QWidget *parent)
: QProgressBar(parent)
{
setWindowTitle(tr("Searching Progress"));
setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

}

