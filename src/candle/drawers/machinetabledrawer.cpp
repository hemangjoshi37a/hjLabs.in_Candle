// This file is a part of "Candle" application.
// Copyright 2015-2025 Hayrullin Denis Ravilevich
//
// hjLabs.in: virtual machine bed/grid plane on Z=0 for spatial context.

#include <cmath>
#include "machinetabledrawer.h"

MachineTableDrawer::MachineTableDrawer()
{
    m_tableSize = QSizeF(300, 300);
    m_minorSpacing = 10.0;
    m_majorSpacing = 50.0;
    m_lineWidth = 1.0;
}

bool MachineTableDrawer::updateData()
{
    m_lines.clear();

    const double w = m_tableSize.width();
    const double h = m_tableSize.height();
    if (w <= 0 || h <= 0 || m_minorSpacing <= 0) return true;

    // Mid-gray palette so the grid reads on BOTH a near-white (light theme)
    // and a dark (dark theme) viewport background: minor grid subtle, major a
    // touch darker, border emphasized. Faint axis tints so origin orientation
    // reads at a glance.
    const QVector3D minorColor(0.62f, 0.62f, 0.62f);
    const QVector3D majorColor(0.50f, 0.50f, 0.50f);
    const QVector3D borderColor(0.42f, 0.42f, 0.42f);
    const QVector3D xAxisColor(0.72f, 0.45f, 0.45f); // faint red toward +X
    const QVector3D yAxisColor(0.45f, 0.72f, 0.45f); // faint green toward +Y

    auto isMultiple = [](double v, double step) {
        return std::fabs(v - std::round(v / step) * step) < 1e-6;
    };

    // Vertical grid lines (constant X), running along Y.
    for (double x = 0; x <= w + 1e-6; x += m_minorSpacing) {
        QVector3D color = isMultiple(x, m_majorSpacing) ? majorColor : minorColor;
        if (std::fabs(x) < 1e-6) color = yAxisColor; // X==0 marks the Y axis
        m_lines.append(VertexData{QVector3D(x, 0, 0), color, QVector3D(), VertexDataTypeLine});
        m_lines.append(VertexData{QVector3D(x, h, 0), color, QVector3D(), VertexDataTypeLine});
    }

    // Horizontal grid lines (constant Y), running along X.
    for (double y = 0; y <= h + 1e-6; y += m_minorSpacing) {
        QVector3D color = isMultiple(y, m_majorSpacing) ? majorColor : minorColor;
        if (std::fabs(y) < 1e-6) color = xAxisColor; // Y==0 marks the X axis
        m_lines.append(VertexData{QVector3D(0, y, 0), color, QVector3D(), VertexDataTypeLine});
        m_lines.append(VertexData{QVector3D(w, y, 0), color, QVector3D(), VertexDataTypeLine});
    }

    // Emphasized outer border.
    const QVector3D corners[4] = {
        QVector3D(0, 0, 0), QVector3D(w, 0, 0), QVector3D(w, h, 0), QVector3D(0, h, 0)
    };
    for (int i = 0; i < 4; i++) {
        m_lines.append(VertexData{corners[i], borderColor, QVector3D(), VertexDataTypeLine});
        m_lines.append(VertexData{corners[(i + 1) % 4], borderColor, QVector3D(), VertexDataTypeLine});
    }

    return true;
}

QSizeF MachineTableDrawer::tableSize() const
{
    return m_tableSize;
}

void MachineTableDrawer::setTableSize(const QSizeF &size)
{
    if (m_tableSize == size) return;
    m_tableSize = size;
    update();
}

double MachineTableDrawer::minorSpacing() const
{
    return m_minorSpacing;
}

void MachineTableDrawer::setMinorSpacing(double spacing)
{
    if (m_minorSpacing == spacing) return;
    m_minorSpacing = spacing;
    update();
}

double MachineTableDrawer::majorSpacing() const
{
    return m_majorSpacing;
}

void MachineTableDrawer::setMajorSpacing(double spacing)
{
    if (m_majorSpacing == spacing) return;
    m_majorSpacing = spacing;
    update();
}
