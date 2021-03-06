//
//  VMDClass.m
//  VMInstrumenter_Sample
//
//  Created by Vittorio Monaco on 12/11/13.
//  Copyright (c) 2013 Vittorio Monaco. All rights reserved.
//

#import "VMDClass.h"
#import "VMDIvar.h"
#import "VMDMethod.h"
#import "VMDProperty.h"

@implementation VMDClass {
    Class _classToInspect;
}

+ (VMDClass *) classWithClass:(Class)classToInspect
{
    VMDClass *wrapper = [VMDClass new];
    wrapper->_classToInspect = classToInspect;
    return wrapper;
}

- (void) addMethodWithSelector:(SEL)selector implementation:(IMP)implementation andSignature:(const char*)signature
{
    class_addMethod(_classToInspect, selector, implementation, signature);
}

- (VMDMethod *) getMethodFromInstanceSelector:(SEL)selector
{
    return [VMDMethod methodWithMethod:class_getInstanceMethod(_classToInspect, selector)];
}

- (VMDMethod *) getMethodFromClassSelector:(SEL)selector
{
    return [VMDMethod methodWithMethod:class_getClassMethod(_classToInspect, selector)];
}

- (VMDMethod *) getMethodFromSelector:(SEL)selector orThrowExceptionWithReason:(NSString *)reason
{
    VMDMethod *methodToReturn = [self getMethodFromInstanceSelector:selector];
    if(!methodToReturn)
    {
        methodToReturn = [self getMethodFromClassSelector:selector];
        if(!methodToReturn)
            @throw [NSException exceptionWithName:NSInternalInconsistencyException
                                       reason:[NSString stringWithFormat:@"%@ - %@",_classToInspect, reason]
                                     userInfo:@{
                                                @"error" : @"Unknown type of selector",
                                                @"info" : NSStringFromSelector(selector)
                                                }];
    }
    
    return methodToReturn;
}

- (BOOL) isInstanceMethod:(SEL)selector
{
    return [self getMethodFromInstanceSelector:selector] != nil;
}

- (BOOL) isClassMethod:(SEL)selector
{
    return [self getMethodFromClassSelector:selector] != nil;
}

- (NSArray *) methods
{
    u_int count;
    Method* methods = class_copyMethodList(_classToInspect, &count);
    NSMutableArray* methodArray = [NSMutableArray arrayWithCapacity:count];
    for (int i = 0; i < count; i++)
    {
        [methodArray addObject:[VMDMethod methodWithMethod:methods[i]]];
    }
    free(methods);
    
    return methodArray;
}

- (NSArray *) properties
{
    u_int count;
    objc_property_t* properties = class_copyPropertyList(_classToInspect, &count);
    NSMutableArray* propertyArray = [NSMutableArray arrayWithCapacity:count];
    for (int i = 0; i < count; i++)
    {
        [propertyArray addObject:[VMDProperty propertyWithObjectiveCProperty:properties[i]]];
    }
    free(properties);
    
    return propertyArray;
}

- (NSArray *) ivars
{
    u_int count;
    Ivar* ivars = class_copyIvarList(_classToInspect, &count);
    NSMutableArray* ivarArray = [NSMutableArray arrayWithCapacity:count];
    for (int i = 0; i < count; i++)
    {
        [ivarArray addObject:[VMDIvar ivarWithIvar:ivars[i]]];
    }
    free(ivars);
    
    return ivarArray;
}

- (Class) metaclass
{
    return object_getClass(_classToInspect);
}

- (Class) underlyingClass
{
    return _classToInspect;
}

@end
