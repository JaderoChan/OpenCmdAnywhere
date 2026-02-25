#include "auto_run_on_startup.h"

#import <Cocoa/Cocoa.h>
#import <ServiceManagement/ServiceManagement.h>

#include <qdebug>

// AI Generate

bool isAutoRunOnStartUp()
{
    @autoreleasepool
    {
        SMAppService *service = SMAppService.mainAppService;
        return service.status == SMAppServiceStatusEnabled;
    }
}

bool setAutoRunOnStartUp(bool enable)
{
    @autoreleasepool
    {
        NSError *error = nil;
        BOOL success = enable
            ? [SMAppService.mainAppService registerAndReturnError:&error]
            : [SMAppService.mainAppService unregisterAndReturnError:&error];

        if (!success)
        {
            if (error)
            {
            qCritical() << QString("[Set Auto Run] failed to %1. Domain: %2, Code: %3, Description: %4.")
                    .arg(enable ? "register" : "unregister")
                    .arg(QString::fromUtf8([error.domain UTF8String]))
                    .arg((long) error.code)
                    .arg(QString::fromUtf8([error.localizedDescription UTF8String]));
            }
            else
            {
                qCritical() << QString("[Set Auto Run] failed to %1 with unknown error.")
                    .arg(enable ? "register" : "unregister");
            }
        }

        return success;
    }
}
