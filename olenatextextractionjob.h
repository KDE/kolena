/*
 * This file is part of the Nepomuk KDE project.
 * Copyright (c) 2009-2010 Sebastian Trueg <trueg@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef _OLENA_TEXT_EXTRACTION_JOB_H_
#define _OLENA_TEXT_EXTRACTION_JOB_H_

#include <KJob>
#include <KUrl>

#include "kolena_export.h"

namespace Nepomuk {
    class KOLENA_EXPORT OlenaTextExtractionJob : public KJob
    {
        Q_OBJECT

    public:
        OlenaTextExtractionJob( QObject* parent = 0 );
        ~OlenaTextExtractionJob();

        QString text() const;

        static OlenaTextExtractionJob* extractText( const KUrl& url );

    public Q_SLOTS:
        void start();
        void cancel();

    private Q_SLOTS:
        void slotFinished();

    private:
        class Private;
        Private* const d;
    };
}

#endif
