#ifndef SETTINGFILE_H
#define SETTINGFILE_H

#include <QWidget>

namespace Ui {
class settingfile;
}

class settingfile : public QWidget
{
    Q_OBJECT

public:
    explicit settingfile(QWidget *parent = 0);
    ~settingfile();
    void Init(void* lib);

private slots:
    void on_read_clicked();

    void on_write_clicked();

private:
    Ui::settingfile *ui;

    void* libHandle;
};

#endif // SETTINGFILE_H
