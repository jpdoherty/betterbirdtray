#ifndef TRAYICON_H
#define TRAYICON_H

#include <QTimer>
#include <QDateTime>
#include <QWidget>
#include <QImage>
#include <QProcess>
#include <QSystemTrayIcon>
#include <QtNetwork/QNetworkConfigurationManager>
#ifdef Q_OS_WIN
#  include "processhandle.h"
#endif /* Q_OS_WIN */
#include "dialogsettings.h"

class UnreadMonitor;
class WindowTools;

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT

    public:
        explicit TrayIcon(bool showSettings);
        ~TrayIcon() override;
        
        /**
         * @return The window tools used by the tray icon.
         */
        WindowTools* getWindowTools() const;
    
        /**
         * @return The unread monitor holding information about the watched mail accounts.
         */
        UnreadMonitor* getUnreadMonitor() const;
    
        /**
         * Hide the Betterbird window.
         */
        void hideBetterbird();
        
        /**
         * Show the Betterbird window.
         */
        void showBetterbird();

    signals:
        void    settingsChanged();

    public slots:
        void    unreadCounterUpdate(unsigned int total, QColor color );

        /**
         * The warning status of a watched path in the unread monitor changed.
         *
         * @param path The path whose warning has changed or a null-string for a global warning.
         */
        void unreadMonitorWarningChanged(const QString &path);
        
        /**
         * Show the settings dialog.
         */
        void showSettings();


    private slots:
        // Updates the icon, this is called in blinking and snooze
        void    updateIcon();

        // Set or reset blinking. Each timeoutms the opacity changes by percentagechange
        // For example, for on-off every 500ms call it with (500, 100)
        void    enableBlinking( bool enabled );

        // Checks the application current state
        void    updateState();

        // Blinking timer
        void    blinkTimeout();

        // Context menu actions
        static void actionQuit();
        void    actionActivate();
        void    actionSnoozeFor();
        void    actionUnsnooze();
        void    actionNewEmail();
        void    actionIgnoreEmails();

        void    actionSystrayIconActivated( QSystemTrayIcon::ActivationReason reason );

        void    startBetterbird();
        
        /**
         * Callback if Betterbird fails to start.
         * @param error The reason for the start failure.
         */
        void    bbProcessError( QProcess::ProcessError error);
        void    bbProcessFinished( int exitCode, QProcess::ExitStatus exitStatus );
#ifdef Q_OS_WIN
        /**
         * Callback if the Betterbird updater exits.
         * @param exitReason The reason for the exit.
         */
        void    bbUpdaterProcessFinished(const ProcessHandle::ExitReason& exitReason);
#endif /* Q_OS_WIN */
        /**
         * Callback that is called when we are about to quit.
         */
        void    onQuit();
        
        /**
         * Called when the auto update finished.
         *
         * @param hasUpdate true if a new update was found.
         * @param errorMessage A message indicating an error during the check, or a null string.
         */
        void    onAutoUpdateCheckFinished(bool foundUpdate, const QString& errorMessage);
        
        /**
         * Called when the Betterbird window is shown.
         */
        void    onBetterbirdWindowShown();
        
        /**
         * Called when the Betterbird window is hidden.
         */
        void    onBetterbirdWindowHidden();

    private:
        void    createMenu();
        void    createUnreadCounterThread();
        
        /**
         * update the number of ignored mails.
         *
         * @param ignoredMails The new number of ignored mails.
         * @param updateIcon Whether a change in the number of ignored mails
         *                   should result in an update of the tray icon.
         */
        void    setIgnoredUnreadMails(unsigned int ignoredMails, bool updateIcon = true);
        
        /**
         * Do an automatic check for a new version of Birdtray.
         */
        void    doAutoUpdateCheck();
        
        /**
         * Draw the warning indicator.
         * @param painter The painter to use when drawing.
         * @param iconSize The size of the icon image to draw on in pixel.
         */
        static void drawWarningIndicator(QPainter &painter, const QSize &iconSize);

        // State variables for blinking; mBlinkingTimeout=0 means we are not blinking
        double          mBlinkingIconOpacity;
        double          mBlinkingDelta;
        unsigned int    mBlinkingTimeout;
        QTimer          mBlinkingTimer;

        // To distinguish whe
        bool            mBlinkTick;

        // Current unread messages count and color
        unsigned int    mUnreadCounter;
        QColor          mUnreadColor;

        // Show/hide Betterbird menu item (we modify its text)
        QAction *       mMenuShowHideBetterbird;

        // Ignore unread emails item (we modify its text) - only if we have this functionality
        QAction *       mMenuIgnoreUnreads;

        // Unsnooze menu item
        QAction *       mMenuUnsnooze;
        QDateTime       mSnoozedUntil;

        // Unread counter thread
        UnreadMonitor * mUnreadMonitor = nullptr;

        // State checking timer (once a second)
        QTimer          mStateTimer;

        // Time when Betterbird could be started
        QDateTime       mBetterbirdStartTime;

        // If true, Betterbird window existed anytime before, but not necessarily now
        // (we use this to distinguish between start and restart)
        bool            mBetterbirdWindowExisted;

        // If true, Betterbird window exists right now
        bool            mBetterbirdWindowExists;

        // If true, it will hide Betterbird window as soon as its shown
        bool            mBetterbirdWindowHide;

        /**
         * The number of unread emails that Birdtray is ignoring.
         */
        unsigned int    ignoredUnreadEmails = 0;

        // Window tools (show/hide)
        WindowTools *   mWinTools;

        // Cached last drawn icon
        QImage          mLastDrawnIcon;

        // Betterbird process which we have started. This can be nullptr if Betterbird
        // was started before Birdtray (thus our process would just activate it and exit)
        // Thus checking this pointer for null doesn't mean TB is not started.
        QProcess    *   mBetterbirdProcess;

        // System tray context menu. Once set, it remains there, so we have to modify existing one
        QMenu       *   mSystrayMenu;
    
        /**
         * The currently opened settings dialog.
         */
        DialogSettings* settingsDialog = nullptr;

#ifdef Q_OS_WIN
        // A reference to a Betterbird updater process.
        ProcessHandle* mBetterbirdUpdaterProcess;
#endif /* Q_OS_WIN */
        
        /**
         * A manager to check for network connectivity.
         */
        QNetworkConfigurationManager* networkConnectivityManager = nullptr;
        
        /**
         * Whether we have received data about unread emails yet.
         */
        bool haveUnreadMailsData = false;
};

#endif // TRAYICON_H
