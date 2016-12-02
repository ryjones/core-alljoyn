/******************************************************************************
 *  * 
 *    Copyright (c) 2016 Open Connectivity Foundation and AllJoyn Open
 *    Source Project Contributors and others.
 *    
 *    All rights reserved. This program and the accompanying materials are
 *    made available under the terms of the Apache License, Version 2.0
 *    which accompanies this distribution, and is available at
 *    http://www.apache.org/licenses/LICENSE-2.0

 ******************************************************************************/

#ifndef ALLJOYN_SECMGR_UTIL_H_
#define ALLJOYN_SECMGR_UTIL_H_

#include <stdint.h>

#include <qcc/Crypto.h>

#include <alljoyn/BusAttachment.h>
#include <alljoyn/Status.h>

using namespace qcc;

namespace ajn {
namespace securitymgr {
/**
 * @brief Utility class to provide a DefaultPolicyMarshaller when needed.
 *        It only requires a BusAttachment and it does not assume any ownership
 *        of the latter.
 */
class Util {
  public:

    /**
     * @brief Initialize the static utility class.
     *
     * @param[in] ba   A previously connected and started BusAttachment.
     *
     * @return ER_OK   If the initialization was successful.
     * @return ER_FAIL Otherwise.
     */
    static QStatus Init(BusAttachment* ba);

    /**
     * @brief Finalize the static utility class. It safely assigns nullptr to local ba.
     *
     * @return ER_OK
     */
    static QStatus Fini();

    /**
     * @brief Return a usable DefaultPolicyMarshaller.
     *
     * @param[in,out] msg    A Message of a BusAttachment that will
     *                       be newly allocated by this function.
     *                       Util has no ownership on the returned Message.
     *
     * @return DefaultPolicyMarshaller A newlly allocated marshaller.
     *                                 Util has no ownership on the returned marshaller.
     * @return nullptr                 Otherwise.
     */
    static DefaultPolicyMarshaller* GetDefaultMarshaller(Message** msg);

    /*@brief Export the byte array representing a permission policy using
     *       a statically fetched default marshaller and message.
     *
     * @param[in] policy           The policy we wish to export.
     * @param[in,out] byteArray    A byte array representing the policy.
     * @param[in,out] size         The size of the byte array returned.
     *
     * @return ER_OK               If the exporting was successful.
     * @return other
     */
    static QStatus GetPolicyByteArray(const PermissionPolicy& policy,
                                      uint8_t** byteArray,
                                      size_t* size);

    /*@brief Creates a permission policy from a byte array
     *       using a statically fetched default marshaller and message.
     *
     * @param[in] byteArray        A byte array representing a policy.
     * @param[in] size             Size of the byte array.
     * @param[in,out] policy       The policy that will be created.
     *
     * @return ER_OK               If the importing was successful.
     * @return other
     */
    static QStatus GetPolicy(const uint8_t* byteArray,
                             const size_t size,
                             PermissionPolicy& policy);

  private:
    static BusAttachment* ba;
    static Mutex mutex;
};
}
}

#endif /* ALLJOYN_SECMGR_UTIL_H_ */