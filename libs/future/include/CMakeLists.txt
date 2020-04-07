# Copyright (c) Microsoft Corporation.
# Licensed under the MIT license.

liblet_includes(
  INCLUDES
    future/cancellationToken.h
    future/details/arrayView.h
    future/details/cancellationErrorProvider.h
    future/details/cancellationException.h
    future/details/executor.h
    future/details/futureFuncInl.h
    future/details/futureInl.h
    future/details/futureTask.h
    future/details/futureWeakPtrInl.h
    future/details/ifuture.h
    future/details/maybeInvoker.h
    future/details/promiseGroupInl.h
    future/details/promiseInl.h
    future/details/resultTraits.h
    future/details/sharedFutureInl.h
    future/details/timeoutException.h
    future/details/whenAllInl.h
    future/details/whenAnyInl.h
    future/future.h
    future/futureForwardDecl.h
    future/futureWait.h
)
