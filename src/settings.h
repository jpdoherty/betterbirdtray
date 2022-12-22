#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSize>
#include <QFont>
#include <QColor>
#include <QMap>
#include <QPixmap>
#include <QSettings>

#include "setting_newemail.h"
#include "utils.h"

class QSettings;

class Settings
{
    public:

        // Icon size constant
        static constexpr int ICON_SIZE = 128;

        Settings();
        ~Settings();

        // Desired icon size
        QSize   mIconSize;

        // Notification icon for unread emails.
        // If null, the mNotificationIcon is used
        QPixmap mNotificationIconUnread;

        // Font for use in notifications
        QFont   mNotificationFont;

        // Notification font weight (0 - 99)
        unsigned int mNotificationFontWeight;

        // Default notification color
        QColor  mNotificationDefaultColor;
        
        /**
         * The border color to use when drawing the unread mail counter.
         */
        QColor mNotificationBorderColor;
    
        /**
         * The width of the border for the unread mail counter.
         */
        unsigned int mNotificationBorderWidth;

        // Blinking speed
        unsigned int    mBlinkSpeed;

        // Opacity level for the tray icon when unread email is present (0.0-1.0)
        double          mUnreadOpacityLevel;

        // The command to start Betterbird. The first element is the executable to launch.
        QStringList mBetterbirdCmdLine;

        // Betterbird window match
        QString mBetterbirdWindowMatch;

        // Whether to show/hide Betterbird on button click
        bool    mShowHideBetterbird;

        // Whether to hide Betterbird when its window is minimized
        bool    mHideWhenMinimized;

        // Whether to launch Betterbird when the app starts
        bool    mLaunchBetterbird;

        // The delay in seconds to launch Betterbird
        int     mLaunchBetterbirdDelay;

        // Whether to hide Betterbird window after starting
        bool    mHideWhenStarted;

        // Whether to quit Betterbird when the app quits
        bool    mExitBetterbirdWhenQuit;

        // Whether to restart Betterbird if it was closed
        bool    mRestartBetterbird;

        // Whether to hide Betterbird window after restarting
        bool    mHideWhenRestarted;

        // Whether to start Betterbird if it is closed and the user clicks on the tray icon
        bool    startClosedBetterbird;
    
        // Whether to hide Betterbird window after starting it via the system tray icon
        bool    hideWhenStartedManually;

        // Whether to monitor Betterbird running
        bool    mMonitorBetterbirdWindow;

        // Whether to use alpha transition when blinking
        bool    mBlinkingUseAlphaTransition;
        
        // Whether to check for a new Birdtray version on startup or not.
        bool    mUpdateOnStartup;
        
        // The new Birdtray version that the user selected to ignore.
        QString mIgnoreUpdateVersion;

        // Whether to allow suppression of unread emails
        bool    mAllowSuppressingUnreads;

        // Whether to show the unread email count
        bool    mShowUnreadEmailCount;
        
        /**
         * Ignore the total number of unread emails that are present at startup.
         */
        bool    ignoreUnreadCountOnStart;
        
        /**
         * Ignore the number of unread emails when showing Betterbird.
         */
        bool    ignoreUnreadCountOnShow;
    
        /**
         * Ignore the number of unread emails when hiding Betterbird.
         */
        bool    ignoreUnreadCountOnHide;
        
        /**
         * Enables or disabled the dialog on startup that shows if no accounts were configured.
         */
        bool    showDialogIfNoAccountsConfigured;
        
        /**
         * Whether to show the Birdtray system tray icon only if there are unread Mail messages.
         */
        bool    onlyShowIconOnUnreadMessages;

        // Watching file timeout (ms)
        unsigned int mWatchFileTimeout;

        // The smallest and the largest allowed font in notification
        unsigned int mNotificationMinimumFontSize;
        unsigned int mNotificationMaximumFontSize;

        // New email data
        bool    mNewEmailMenuEnabled;
        QList< Setting_NewEmail >   mNewEmailData;

        /**
         * A mapping of watched mork files to their notification color
         * in the order the user added them.
         */
        OrderedMap<QString, QColor> watchedMorkFiles;

        // If non-zero, specifies an interval in seconds for rereading index files even if they didn't change. 0 disables.
        unsigned int    mIndexFilesRereadIntervalSec;

        // When the number of unread emails changes, Birdtray can start this process
        QString                   mProcessRunOnCountChange;

        // Whether to support non-compliant NetWM WMs by ignoring NETWM hints
        bool                      mIgnoreNETWMhints;

        // Load and save them
        void    save();
        void    load();
        
        /**
         * @param executable The actual executable necessary to start (not necessary Betterbird itself - may be shell or script).
         * @param arguments The arguments necessary to start Betterbird.
         * @return true if executable/arguments is set, false if error
         */
        bool getStartBetterbirdCmdline( QString& executable, QStringList &arguments );
        
        /**
         * @return The icon to use for the system tray.
         */
        const QPixmap& getNotificationIcon();
        
        /**
         * Set the icon to use for the system tray to the new icon.
         *
         * @param icon The new icon.
         */
        void setNotificationIcon(const QPixmap& icon);

    private:
        Q_DECLARE_TR_FUNCTIONS(Settings)
    
        // Loading from either storage
        void    fromJSON( const QJsonObject& settings );

        // TODO: remove this on March 23, 2022
        void    fromQSettings(QSettings * settings);

        // Notification icon
        QPixmap     mNotificationIcon;
    
        // Settings filename
        QString     mSettingsFilename;

        /**
         * At first start, load the configuration configured during installation of Birdtray.
         */
        void loadInstallerConfiguration();
};

#endif // SETTINGS_H
