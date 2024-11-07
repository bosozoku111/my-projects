#ifndef XORAPP_H
#define XORAPP_H

#include <QWidget>

class xorApp : public QWidget
{
    Q_OBJECT
public:
    xorApp();
private slots:
    void openInputBrowse();
    void openOutputBrowse();
    void xorModification();
private:
    void xorOperation(std::fstream& inputFile, std::fstream& outputFile);
    int m_iFileIndex;
};
#endif
