// This file is a part of "Candle" application.
// Copyright 2015-2025 Hayrullin Denis Ravilevich
//
// hjLabs.in: virtual machine bed/grid plane drawn on Z=0 for spatial
// context (like a 3D-printer bed). Always visible, kept visually quiet so
// toolpaths and the tool stand out. Modelled on MachineBoundsDrawer.

#ifndef MACHINETABLEDRAWER_H
#define MACHINETABLEDRAWER_H

#include <QSizeF>
#include "shaderdrawable.h"

class MachineTableDrawer : public ShaderDrawable
{
public:
    MachineTableDrawer();

    // Work area size in mm (X = width, Y = height). Defaults to 300x300.
    QSizeF tableSize() const;
    void setTableSize(const QSizeF &size);

    double minorSpacing() const;
    void setMinorSpacing(double spacing);

    double majorSpacing() const;
    void setMajorSpacing(double spacing);

protected:
    bool updateData() override;

private:
    QSizeF m_tableSize;
    double m_minorSpacing;
    double m_majorSpacing;
};

#endif // MACHINETABLEDRAWER_H
