#ifndef MACROS_H
#define MACROS_H

#define TEXT(widget) widget->currentText().toLatin1().data()

/*
 * Typically used when a widget like a QComboBox contains translated text
 * which obviously would not be very good to feed to rc.xml and we therefore
 * need the QVariant userdata instead.
 */
#define DATA(widget) widget->currentData().toString().toLatin1().data()

#endif // MACROS_H
