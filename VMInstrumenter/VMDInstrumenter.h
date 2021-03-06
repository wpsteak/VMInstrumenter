//
//  VMDInstrumenter.h
//  VMDInstrumenter_Sample
//
//  Created by Vittorio Monaco on 28/08/13.
//  Copyright (c) 2013 Vittorio Monaco. All rights reserved.
//

#import <Foundation/Foundation.h>

// constants and typedefs for blocks

extern const NSString * VMDInstrumenterDefaultMethodExceptionReason;

typedef void(^VMDExecuteBefore)(id);
typedef void(^VMDExecuteAfter)(id);
typedef BOOL(^VMDTestBlock)(id);

typedef NS_OPTIONS(NSUInteger, VMDInstrumenterTracingOptions)
{
    VMDInstrumenterTracingOptionsNone   = 0,
    VMDInstrumenterDumpStacktrace       = 1 << 0,
    VMDInstrumenterDumpObject           = 1 << 1,
    VMDInstrumenterTraceExecutionTime   = 1 << 2,
    VMDInstrumenterTracingOptionsAll    = 7
};

@interface VMDInstrumenter : NSObject

/**
 Access the shared VMDInstrumenter instance
 */
+ (instancetype) sharedInstance;

/**
 Instance methods
 */

/**
 This method suppresses every call to the specified selector of the specified class
 This means that every time someone (also in 3rd party frameworks) makes a call to the selector,
 NOTHING will happen. Be warned
 
 @param selectorToSuppress the selector to suppress
 @param classToInspect the class on which you want to suppress the selector
 */
- (void) suppressSelector:(SEL)selectorToSuppress forClass:(Class)classToInspect;

/**
 This method just reverts what suppressSelector:forClass: does
 
 @param selectorToRestore the selector you want to restore
 @param classToInspect the class for which you want to restore the selector
 
 @discussion if the method is not suppressed, this will throw an exception because it means something went wrong at some point
 @throws NSInternalInconsistencyException if the selector is not suppressed
 */
- (void) restoreSelector:(SEL)selectorToRestore forClass:(Class)classToInspect;

/**
 This method replaces the implementation of the two specified selectors from the two specified classes
 
 @discussion From the time you call this method, every call to sel1 will be treated as a call to sel2 and viceversa
 
 @param sel1 the selector to replace
 @param class1 the class where sel1 is taken
 @param sel2 the selector to replace with sel1
 @param class2 the class where sel2 is taken
 */
- (void) replaceSelector:(SEL)sel1 ofClass:(Class)class1 withSelector:(SEL)sel2 ofClass:(Class)class2;

/**
 This method instruments calls to a specified selector of a specified class and does something
 with beforeBlock and afterBlock parameters. Specifically, every time the selector is called on any instance of the class passed,
 beforeBlock is executed before the selector is, and afterBlock is executed after the selector is.
 
 @param selectorToInstrument the selector that you'd like to instrument
 @param classToInspect the class to take the selector from
 @param beforeBlock the block of code to execute before the call to the selector
 @param afterBlock the block of code to execute after the call to the selector
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the specified class
 */
- (void) instrumentSelector:(SEL)selectorToInstrument forClass:(Class)classToInspect withBeforeBlock:(VMDExecuteBefore)beforeBlock afterBlock:(VMDExecuteAfter)afterBlock;

/**
 This method instruments calls to a specified selector on a specified object
 with beforeBlock and afterBlock parameters. Specifically, every time the selector is called on the instance,
 beforeBlock is executed before the selector is, and afterBlock is executed after the selector is.
 
 @param selectorToInstrument the selector that you'd like to instrument
 @param objectInstance the instance that gets the selector called on
 @param beforeBlock the block of code to execute before the call to the selector
 @param afterBlock the block of code to execute after the call to the selector
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the class of the specified object
 */
- (void) instrumentSelector:(SEL)selectorToInstrument forObject:(id)objectInstance withBeforeBlock:(VMDExecuteBefore)beforeBlock afterBlock:(VMDExecuteAfter)afterBlock;

/**
 This method instruments calls to a specified selector on a specified object
 with beforeBlock and afterBlock parameters. Specifically, every time the selector is called on the instance,
 beforeBlock is executed before the selector is, and afterBlock is executed after the selector is.
 
 @param selectorToInstrument the selector that you'd like to instrument
 @param classToInspect the class to take the selector from
 @param testBlock the test block that the instances of the class have to pass to be traced
 @param beforeBlock the block of code to execute before the call to the selector
 @param afterBlock the block of code to execute after the call to the selector
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the class of the specified object
 */
- (void) instrumentSelector:(SEL)selectorToInstrument forInstancesOfClass:(Class)classToInspect passingTest:(VMDTestBlock)testBlock withBeforeBlock:(VMDExecuteBefore)beforeBlock afterBlock:(VMDExecuteAfter)afterBlock;

/**
 This method instruments calls to a specified selector of a specified class and just logs execution
 
 You can use more specific methods if you want particular tracing to be done
 
 @param selectorToTrace the selector that you'd like to trace
 @param classToInspect the class to take the selector from
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the specified class
 */
- (void) traceSelector:(SEL)selectorToTrace forClass:(Class)classToInspect;

/**
 This method instruments calls to a specified selector on a specified object
 
 You can use more specific methods if you want particular tracing to be done
 
 @param selectorToTrace the selector that you'd like to trace
 @param objectInstance the instance that gets the selector called on
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the class of the specified object
 */
- (void) traceSelector:(SEL)selectorToTrace forObject:(id)objectInstance;

/**
 This method instruments calls to a specified selector of a specified class
 but only for instances of the class that pass the specified testBlock and just logs execution as the previous method
 
 @param selectorToTrace the selector that you'd like to trace
 @param classToInspect the class to take the selector from
 @param testBlock the test block that the instances of the class have to pass to be traced
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the specified class
 */
- (void) traceSelector:(SEL)selectorToTrace forInstancesOfClass:(Class)classToInspect passingTest:(VMDTestBlock)testBlock;

/**
 This method instruments calls to a specified selector of a specified class and just logs execution as the previous method
 Moreover, if dumpStack is YES, it prints the stack trace after every execution
 
 @param selectorToTrace the selector that you'd like to trace
 @param classToInspect the class to take the selector from
 @param options you can choose what you want apart from tracing here (stacktrace, dump of self object, method execution time)
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the specified class
 */
- (void) traceSelector:(SEL)selectorToTrace forClass:(Class)classToInspect withTracingOptions:(VMDInstrumenterTracingOptions)options;

/**
 This method instruments calls to a specified selector of a specified object and just logs execution as the previous method
 Moreover, if dumpStack is YES, it prints the stack trace after every execution
 
 @param selectorToTrace the selector that you'd like to trace
 @param objectInstance the instance that gets the selector called on
 @param options you can choose what you want apart from tracing here (stacktrace, dump of self object, method execution time)
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the class of the specified object
 */
- (void) traceSelector:(SEL)selectorToTrace forObject:(id)objectInstance withTracingOptions:(VMDInstrumenterTracingOptions)options;

/**
 This method instruments calls to a specified selector of a specified class
 but only for instances of the class that pass the specified testBlock and just logs execution as the previous method
 
 @param selectorToTrace the selector that you'd like to trace
 @param classToInspect the class to take the selector from
 @param testBlock the test block that the instances of the class have to pass to be traced
 @param options you can choose what you want apart from tracing here (stacktrace, dump of self object, method execution time)
 
 @throws NSInternalInconsistencyException Just in case the selector cannot be found in the specified class
 */
- (void) traceSelector:(SEL)selectorToTrace forInstancesOfClass:(Class)classToInspect passingTest:(VMDTestBlock)testBlock withTracingOptions:(VMDInstrumenterTracingOptions)options;

@end
