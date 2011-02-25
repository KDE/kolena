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

#include "olenatextextractionjob.h"

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QFile>

#include <KIO/NetAccess>
#include <KDebug>

#include <string>

#include <mln/io/ppm/load.hh>
#include <mln/io/ppm/save.hh>
#include <mln/labeling/colorize.hh>

#include "text_extraction.hh"

namespace {
/**
 * We check two basic things:
 * 1. is there any text at all
 * 2. Is the letter/non-letter ratio useful - this is a primitive attempt to exclude garbage such as " W Y œe "ii" ï§ _* ,"
 */
bool checkText( const QString& text ) {
    if(!text.isEmpty()) {
        int letterCnt = 0;
        int spaceCnt = 0;
        Q_FOREACH(const QChar& c, text) {
            if(c.isLetterOrNumber())
                ++letterCnt;
            else if(c.isSpace())
                ++spaceCnt;
        }
        double letterRatio = double(letterCnt)/double(text.length());
        double spaceRatio = double(spaceCnt)/double(text.length());
        kDebug() << "Letter ration of" << text << letterRatio << spaceRatio;
        return letterRatio > 0.7 && spaceRatio < 0.3;
    }
    else {
        return false;
    }
}
}

class Nepomuk::OlenaTextExtractionJob::Private : public QThread
{
public:
    KUrl m_url;
    QString m_tmpFilePath;

    QString m_extractedText;

private:
    void run();
};


void Nepomuk::OlenaTextExtractionJob::Private::run()
{
    m_extractedText = scribo::toolchain::nepomuk::text_extraction( QImage( m_tmpFilePath ) );
    kDebug() << m_extractedText;
    if(!checkText(m_extractedText)) {
        kDebug() << "Extracted text seems to be junk.";
        m_extractedText.truncate(0);
    }
}


Nepomuk::OlenaTextExtractionJob::OlenaTextExtractionJob( QObject* parent )
    : KJob( parent ),
      d( new Private() )
{
    connect( d, SIGNAL( finished() ), this, SLOT( slotFinished() ) );
}


Nepomuk::OlenaTextExtractionJob::~OlenaTextExtractionJob()
{
    d->wait();
    delete d;
}


Nepomuk::OlenaTextExtractionJob* Nepomuk::OlenaTextExtractionJob::extractText( const KUrl& url )
{
    OlenaTextExtractionJob* job = new OlenaTextExtractionJob();
    job->d->m_url = url;
    return job;
}


void Nepomuk::OlenaTextExtractionJob::start()
{
    kDebug();
    if ( KIO::NetAccess::download( d->m_url, d->m_tmpFilePath, 0 ) ) {
        kDebug() << "Starting thread";
        d->start();
    }
    else {
        kDebug() << "Failed to download" << d->m_url << KIO::NetAccess::lastErrorString();
        emitResult();
    }
}


void Nepomuk::OlenaTextExtractionJob::cancel()
{
}


QString Nepomuk::OlenaTextExtractionJob::text() const
{
    return d->m_extractedText;
}


void Nepomuk::OlenaTextExtractionJob::slotFinished()
{
    KIO::NetAccess::removeTempFile( d->m_tmpFilePath );
    emitResult();
}

#include "olenatextextractionjob.moc"
