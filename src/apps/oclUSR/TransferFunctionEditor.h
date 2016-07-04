/* Copyright (c) 2015, EPFL/Blue Brain Project
 *                     Marwan Abdellah <marwan.abdellah@epfl.ch>
 *                     Grigori Chevtchenko <grigori.chevtchenko@epfl.ch>
 *
 * This file is part of Livre <https://github.com/BlueBrain/Livre>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef _TransferFunctionEditor_h_
#define _TransferFunctionEditor_h_

#include <QWidget>
#include <Utilities.h>
namespace Ui
{
class TransferFunctionEditor;
}

class ColorMapWidget;

/**
 * This contains all the widget for the transfert function editor.
 */
class TransferFunctionEditor: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor of TransferFunctionEditor.
     * @param conroller The controller used to receive/publish transfer fuction
     * data.
     * @param tfParentWidget Parent widget.
     */
    TransferFunctionEditor(QWidget* tfParentWidget = 0 );
    ~TransferFunctionEditor();

Q_SIGNALS:
    void gradientStopsChanged( const QGradientStops& stops );
    void transferFunctionChanged( UInt8Vector tf );

private Q_SLOTS:

    void _clear();
    void _load();
    void _save();
    void _setDefault();
    void _pointsUpdated();
    void _onTransferFunctionChanged( UInt8Vector tf );

private:
    void _publishTransferFunction();

private:
    Ui::TransferFunctionEditor* _ui;
    bool _tfReceived;
    ColorMapWidget* _redWidget;
    ColorMapWidget* _greenWidget;
    ColorMapWidget* _blueWidget;
    ColorMapWidget* _alphaWidget;
};


#endif // _TransferFunctionEditor_h_
