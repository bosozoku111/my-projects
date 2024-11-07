#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QIntValidator>
#include <QMessageBox>
#include <QFileDialog>
#include <fstream>
#include <cstdio>

#include "xorApp.h"

xorApp::xorApp() : m_iFileIndex(0)
{
    setWindowTitle("XOR File Modifier");
    QVBoxLayout* pMainLayout = new QVBoxLayout(this);

    QLineEdit* pInputMaskLineEdit = new QLineEdit(this);
    pInputMaskLineEdit->setPlaceholderText("Введите маску входных данных (например, .txt)");
    pMainLayout->addWidget(pInputMaskLineEdit);

    QHBoxLayout* pInputFileHLayout = new QHBoxLayout;
    QLineEdit* pInputFileLineEdit = new QLineEdit(this);
    pInputFileLineEdit->setPlaceholderText("Выберите исходный файл");
    QPushButton* pBrowseInputButton = new QPushButton("Выбрать файл", this);
    pInputFileHLayout->addWidget(pInputFileLineEdit);
    pInputFileHLayout->addWidget(pBrowseInputButton);
    pMainLayout->addLayout(pInputFileHLayout);
    connect(pBrowseInputButton, SIGNAL(clicked()), SLOT(openInputBrowse()));

    QCheckBox* pDeleteInputFilesCheckBox = new QCheckBox("Удалять входные файлы", this);
    pMainLayout->addWidget(pDeleteInputFilesCheckBox);

    QHBoxLayout* pPathHLayout = new QHBoxLayout();
    QLineEdit* pOutputPathLineEdit = new QLineEdit(this);
    pOutputPathLineEdit->setPlaceholderText("Введите путь для сохранения");
    QPushButton* pBrowseButton = new QPushButton("Обзор", this);
    pPathHLayout->addWidget(pOutputPathLineEdit);
    pPathHLayout->addWidget(pBrowseButton);
    pMainLayout->addLayout(pPathHLayout);
    connect(pBrowseButton, SIGNAL(clicked()), SLOT(openOutputBrowse()));

    QGridLayout* pXorGridLayout = new QGridLayout;
    QLabel* pLabelTextInput = new QLabel("Введите 8-байтную переменную:", this);
    pMainLayout->addWidget(pLabelTextInput);
    for (int i = 0; i < 8; i++)
    {
        QLineEdit* pByteLineEdit = new QLineEdit(this);
        pByteLineEdit->setFixedWidth(35);
        pByteLineEdit->setValidator(new QIntValidator(0, 255));
        pXorGridLayout->addWidget(pByteLineEdit, 0, i);
    }
    pMainLayout->addLayout(pXorGridLayout);
    QPushButton* pProcessDataButton = new QPushButton("Обработать файл", this);
    pMainLayout->addWidget(pProcessDataButton);
    connect(pProcessDataButton, SIGNAL(clicked()), SLOT(xorModification()));
}

void xorApp::openInputBrowse()
{
    QLineEdit* pFileMaskLine = static_cast<QLineEdit*>(layout()->itemAt(0)->widget());
    QString qsInputFilePath = QFileDialog::getOpenFileName(nullptr, "Choose file", "", "File Type (*" + pFileMaskLine->text() + ")");
    if (!qsInputFilePath.isEmpty())
    {
        QHBoxLayout* pHLayout = static_cast<QHBoxLayout*>(layout()->itemAt(1));
        QLineEdit* pLineEdit = static_cast<QLineEdit*>(pHLayout->itemAt(0)->widget());
        pLineEdit->setText(qsInputFilePath);
    }
    else
        QMessageBox::warning(this, "Предупреждение", "Выбор файла отменен.");
}

void xorApp::openOutputBrowse()
{
    QString qsOutputFilePath = QFileDialog::getExistingDirectory(this, "Выбрать директорию");
    if (!qsOutputFilePath.isEmpty())
    {
        QHBoxLayout* pHLayout = static_cast<QHBoxLayout*>(layout()->itemAt(3));
        QLineEdit* pLineEdit = static_cast<QLineEdit*>(pHLayout->itemAt(0)->widget());
        pLineEdit->setText(qsOutputFilePath);
    }
    else
        QMessageBox::warning(this, "Предупреждение", "Путь для сохранения файла не был выбран.");
}

void xorApp::xorOperation(std::fstream& inputFile, std::fstream& outputFile)
{
    QHBoxLayout* pHLayout = static_cast<QHBoxLayout*>(layout()->itemAt(5));
    QLineEdit* byteLine;
    uint8_t key[8];
    for (int i = 0; i < 8; i++)
    {
        byteLine = static_cast<QLineEdit*>(pHLayout->itemAt(i)->widget());
        key[i] = static_cast<uint8_t>(byteLine->text().toInt());
    }
    char buffer;
    int bytesCounter = 0;
    while(inputFile.get(buffer))
    {
        buffer ^= key[bytesCounter % 8];
        outputFile.put(buffer);
        bytesCounter++;
    }
}

void xorApp::xorModification()
{
    QHBoxLayout* pHLayout = static_cast<QHBoxLayout*>(layout()->itemAt(1));
    QLineEdit* pFilepathLineEdit = static_cast<QLineEdit*>(pHLayout->itemAt(0)->widget());
    const std::string sInFilepath = pFilepathLineEdit->text().toStdString();
    std::fstream inputFile(sInFilepath, std::ios::in | std::ios::binary);
    pHLayout = static_cast<QHBoxLayout*>(layout()->itemAt(3));
    pFilepathLineEdit = static_cast<QLineEdit*>(pHLayout->itemAt(0)->widget());

    QLineEdit* pFileMaskLineEdit = static_cast<QLineEdit*>(layout()->itemAt(0)->widget());
    QString qsOutFilePath = pFilepathLineEdit->text();
    QFileInfo qInputFileInfo(sInFilepath.c_str());
    do
    {
        m_iFileIndex++;
        qsOutFilePath = qsOutFilePath + "/" + qInputFileInfo.baseName() + QString::number(m_iFileIndex) + pFileMaskLineEdit->text();
    } while (QFile::exists(qsOutFilePath));
    std::fstream outputFile(qsOutFilePath.toStdString(), std::ios::out | std::ios::binary);
    if  (inputFile.is_open())
    {
        if (outputFile.is_open())
        {
            xorOperation(inputFile, outputFile);
            inputFile.close();
            outputFile.close();
            QCheckBox* checkBox = static_cast<QCheckBox*>(layout()->itemAt(2)->widget());
            if(checkBox->isChecked())
                std::remove(sInFilepath.c_str());
            QMessageBox::information(this, "Информация", "Операция прошла успешно.");
        }
        else
            QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для записи.");
    }
    else
        QMessageBox::warning(this, "Ошибка", "Не удалось открыть файл для чтения.");
}
