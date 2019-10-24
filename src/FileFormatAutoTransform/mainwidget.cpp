#include "mainwidget.h"
#include "ui_mainwidget.h"
#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
#include <QFileInfoList>
#include <QFileInfo>
#include <QTextStream>

MainWidget::MainWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MainWidget)
{
    ui->setupUi(this);
}

MainWidget::~MainWidget()
{
    delete ui;
}

void MainWidget::on_btn_addFileFormat_clicked()
{
    QString strFileCodeFormat = ui->edit_fileCodeFormat->text();

    QStringList list = strFileCodeFormat.split(",");
    if (list.length() <= 0)
    {
        QMessageBox::warning(this, tr("Warm"), tr("Input file code format!"));
        ui->edit_fileCodeFormat->setText("");
        return;
    }

    for (int i=0; i<list.size(); i++)
    {
        QString strFormat = list.at(i);
        QTextCodec *codec = QTextCodec::codecForName(strFormat.toLatin1());
        if (codec)
        {
            int iTableRowCnt = ui->table_fileCodeFormat->rowCount();
            ui->table_fileCodeFormat->insertRow(iTableRowCnt);
            QTableWidgetItem *item = new QTableWidgetItem(strFormat);
            ui->table_fileCodeFormat->setItem(iTableRowCnt, 0, item);
        }
    }

    ui->edit_fileCodeFormat->setText("");
}

void MainWidget::on_btn_deleteFileFormat_clicked()
{
    int iTableRowCnt = ui->table_fileCodeFormat->rowCount();
    for (int i=iTableRowCnt-1; i>=0; i--)
    {
        if (ui->table_fileCodeFormat->item(i, 0)->isSelected())
        {
            QTableWidgetItem *item = ui->table_fileCodeFormat->takeItem(i, 0);
            delete item;
            ui->table_fileCodeFormat->removeRow(i);
        }
    }
}

void MainWidget::on_btn_addFileSuffix_clicked()
{
    QString strFileSuffix = ui->edit_fileSuffix->text();

    QStringList list = strFileSuffix.split(",");
    if (list.length() <= 0)
    {
        QMessageBox::warning(this, tr("Warm"), tr("Input file suffix!"));
        ui->edit_fileSuffix->setText("");
        return;
    }

    for (int i=0; i<list.size(); i++)
    {
        QString strSuffix = list.at(i);
        if (strSuffix.length() > 0)
        {
            int iTableRowCnt = ui->table_fileSuffix->rowCount();
            ui->table_fileSuffix->insertRow(iTableRowCnt);
            QTableWidgetItem *item = new QTableWidgetItem(strSuffix);
            ui->table_fileSuffix->setItem(iTableRowCnt, 0, item);
        }
    }

    ui->edit_fileSuffix->setText("");
}

void MainWidget::on_btn_deleteFileSuffix_clicked()
{
    int iTableRowCnt = ui->table_fileSuffix->rowCount();
    for (int i=iTableRowCnt-1; i>=0; i--)
    {
        if (ui->table_fileSuffix->item(i, 0)->isSelected())
        {
            QTableWidgetItem *item = ui->table_fileSuffix->takeItem(i, 0);
            delete item;
            ui->table_fileSuffix->removeRow(i);
        }
    }
}

void MainWidget::on_btn_selectFilePath_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this,
                                                       tr("Open Directory"),
                                                       ".",
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);
    ui->edit_filePath->setText(strDir);
}

void MainWidget::on_btn_exec_clicked()
{
    QString strDir = ui->edit_filePath->text();
    if (strDir.length() <= 0)
    {
        QMessageBox::warning(this, tr("Warm"), tr("Input dir path!"));
        return;
    }

    ui->edit_filePath->setText("");

    QStringList fileFormats = getFileFormat();
    QList<QTextCodec*> listCodec;
    for (int i=0; i<fileFormats.size(); i++)
    {
        QTextCodec *codec = QTextCodec::codecForName(fileFormats.at(i).toStdString().c_str());
        if (codec)
            listCodec.push_back(codec);
    }

    QStringList files = getFileName(strDir, getFileSuffixex());
    for (int i=0; i<files.size(); i++)
    {
        QString strFile = files.at(i);
        QFile file(strFile);
        if (file.open(QIODevice::ReadWrite))
        {
            QByteArray data = file.readAll();
            file.close();

            for (int i=0; i<listCodec.size(); i++)
            {
                QTextCodec::ConverterState state;
                QString text = listCodec.at(i)->toUnicode(data.constData(), data.size(), &state);
                if (state.invalidChars <= 0)
                {
                    if (file.open(QIODevice::ReadWrite | QIODevice::Truncate))
                    {
                        QTextStream stream(&file);
                        stream.setCodec(QTextCodec::codecForName(ui->edit_destCodeFormat->text().toStdString().c_str()));
                        stream << text.toUtf8();

                        file.close();
                        break;
                    }
                }
            }
        }
    }
}

QStringList MainWidget::getFileName(const QString &path, const QStringList &fileSuffix)
{
    QStringList retList;

    QDir dir(path);

    QFileInfoList fileInfoList = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    foreach (const QFileInfo& value, fileInfoList)
    {
        // 文件夹
        if (value.isDir())
        {
            retList.append(getFileName(value.absoluteFilePath(), fileSuffix));
        }
        // 文件
        else if (value.isFile())
        {
            QString fileName = value.absoluteFilePath();

            foreach (const QString& value, fileSuffix)
            {
                if (fileName.endsWith(value))
                {
                    retList.push_back(fileName);
                    break;
                }
            }
        }
    }

    return retList;
}

QStringList MainWidget::getFileSuffixex() const
{
    QStringList suffixes;

    for (int i=0; i<ui->table_fileSuffix->rowCount(); i++)
    {
        QTableWidgetItem *item = ui->table_fileSuffix->item(i, 0);
        if (item && item->text().length()>0)
            suffixes.push_back(item->text());
    }

    return suffixes;
}

QStringList MainWidget::getFileFormat() const
{
    QStringList formats;

    for (int i=0; i<ui->table_fileCodeFormat->rowCount(); i++)
    {
        QTableWidgetItem *item = ui->table_fileCodeFormat->item(i, 0);
        if (item && item->text().length()>0)
            formats.push_back(item->text());
    }

    return formats;
}
