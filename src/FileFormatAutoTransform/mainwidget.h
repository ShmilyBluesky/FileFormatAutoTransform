#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

namespace Ui {
class MainWidget;
}

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private slots:
    void on_btn_addFileFormat_clicked();

    void on_btn_deleteFileFormat_clicked();

    void on_btn_addFileSuffix_clicked();

    void on_btn_deleteFileSuffix_clicked();

    void on_btn_selectFilePath_clicked();

    void on_btn_exec_clicked();

private:
    // 获取所有文件
    QStringList getFileName(const QString& path, const QStringList& fileSuffix);
    QStringList getFileSuffixex() const;
    QStringList getFileFormat() const;

private:
    Ui::MainWidget *ui;
};

#endif // MAINWIDGET_H
