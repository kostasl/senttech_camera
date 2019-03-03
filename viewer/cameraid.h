#ifndef CAMERAID_H
#define CAMERAID_H

#include <QWidget>

namespace Ui {
class cameraid;
}

class cameraid : public QWidget
{
    Q_OBJECT

public:
    explicit cameraid(QWidget *parent = 0);
    ~cameraid();
    void Init(int fd);

private slots:
    void on_read_clicked();

    void on_write_clicked();

private:
    Ui::cameraid *ui;

    int devHandle;
};

#endif // CAMERAID_H
