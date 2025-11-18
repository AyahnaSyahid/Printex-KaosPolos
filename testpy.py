from PySide6.QtPrintSupport import QPrinter, QPrinterInfo
from PySide6.QtCore import QCoreApplication, QSizeF, QPointF
from PySide6.QtGui import QGuiApplication, QPainter, QPageSize, QImage, QFont


def test1():
    app = QGuiApplication([])
    pinfo = None
    for p in QPrinterInfo.availablePrinters():
        if "Foxit" in p.printerName():
            printer = p
    if p is None:
        app.exit()
    
    printer = QPrinter(p)
    printer.setResolution(300)
    printer.setPageSize(QPageSize(QSizeF(80, 500), QPageSize.Millimeter, "RollSource", QPageSize.ExactMatch))
    printer.setOutputFileName("testPrint.pdf")
    painter = QPainter(printer)
    painter.drawText(QPointF(0, 0), "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX")
    painter.end()
    app.exit()
    

if __name__ == "__main__":
    test1()