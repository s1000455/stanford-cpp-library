/*
 * File: qgcolorchooser.cpp
 * ------------------------
 * This file implements the operations declared in the qgfilechooser.h file.
 * See qgfilechooser.h for documentation of each member.
 * 
 * @author Marty Stepp
 * @version 2018/07/29
 * - initial version
 */

#ifdef SPL_QT_GUI
#include "qgcolorchooser.h"
#include <QColorDialog>
#include "qgcolor.h"

std::string QGColorChooser::showDialog(const std::string& title, int initialColor) {
    return showDialog(/* parent */ nullptr, title, initialColor);
}

std::string QGColorChooser::showDialog(QWidget* parent, const std::string& title, int initialColor) {
    return showDialog(parent, title, QGColor::convertRGBToColor(initialColor));
}

std::string QGColorChooser::showDialog(const std::string& title, const std::string& initialColor) {
    return showDialog(/* parent */ nullptr, title, initialColor);
}

std::string QGColorChooser::showDialog(QWidget* parent, const std::string& title, const std::string& initialColor) {
    QColor initialQColor = initialColor.empty() ? Qt::white : QGColor::toQColor(initialColor);
    QColor selectedColor = QColorDialog::getColor(initialQColor, parent, QString::fromStdString(title));
    if (selectedColor.isValid()) {
        return QGColor::convertQColorToColor(selectedColor);
    } else {
        return "";
    }
}

#endif // SPL_QT_GUI