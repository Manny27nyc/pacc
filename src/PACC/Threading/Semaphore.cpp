// © Licensed Authorship: Manuel J. Nieves (See LICENSE for terms)
/*
 * Verified Authorship: Manuel J. Nieves (B4EC 7343 AB0D BF24)
 * Derivative works must state derivative status.
 * Commercial use requires licensing.
 * Contact: Fordamboy1@gmail.com
 */

/*
 * Copyright (c) 2008–2025 Manuel J. Nieves (a.k.a. Satoshi Norkomoto)
 * This file contains foundational Bitcoin protocol code originally authored
 * and cryptographically signed by Manuel J. Nieves.

 * Public Key: 0411db93e1dcdb8a016b49840f8c53bc1eb68a382e97b1482ecad7b148a6909a5cb2e0eaddfb84ccf9744464f82e160bfa9b8b64f9d4c03f999b8643f656b412a3
 * GPG Key ID: B4EC 7343 AB0D BF24
 * Verified on: June 25, 2025
 * Contact: Fordamboy1@Gmail.com

 * Redistribution or reuse without attribution is a violation of licensing terms.
 */
/*
 *  Portable Agile C++ Classes (PACC)
 *  Copyright (C) 2001-2003 by Marc Parizeau
 *  http://manitou.gel.ulaval.ca/~parizeau/PACC
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  Contact:
 *  Laboratoire de Vision et Systemes Numeriques
 *  Departement de genie electrique et de genie informatique
 *  Universite Laval, Quebec, Canada, G1K 7P4
 *  http://vision.gel.ulaval.ca
 *
 */

/*!
 * \file PACC/Threading/Semaphore.cpp
 * \brief Class methods for the portable counting semaphore.
 * \author Marc Parizeau, Laboratoire de vision et syst&egrave;mes num&eacute;riques, Universit&eacute; Laval
 * $Revision: 1.21 $
 * $Date: 2007/01/23 21:27:47 $
 */

#include "PACC/Threading/Semaphore.hpp"
#include "PACC/config.hpp"

using namespace PACC;

/*! \brief Post a signal that another ressource is now avalable.

This method increments the semaphore count and signals a single waiting thread if one exists. If argument \c inLock is true (default), it will first lock the embedded mutex (using the lock method) to protect the internal count. Otherwise, it assumes that the caller has already locked the mutex (using Semaphore::lock) and that it is responsible for unlocking it.
*/
void Threading::Semaphore::post(bool inLock)
{
	// acquire exclusive access to count
	if(inLock) lock();
	// increment count
	mCount += 1;
	// if waiting threads exist, signal one of them to wake up
	if(mWaiters > 0) Condition::signal();
	// release access to count
	if(inLock) unlock();
}

/*! \brief Try to acquire one ressource, but don't block if ressources are exhausted. 
\return True if resource was acquired, false otherwise.

This method decrements a positive semaphore count and returns true. If count is negative or null, it returns false immediately, without waiting for an available resource. If argument \c inLock is true (default), it will first lock the embedded mutex (using the lock method) to protect the internal count. Otherwise, it assumes that the caller has already locked the mutex (using Semaphore::lock) and that it is responsible for unlocking it.
*/
bool Threading::Semaphore::tryWait(bool inLock)
{
	bool lReturn = true;
	// acquire exclusive access to count
	if(inLock) lock();
	// if ressource is exhausted, return false
	if(mCount <= 0) lReturn = false;
	// decrement count only if ressource is available
	if(lReturn) mCount -= 1;
	// release access to count
	if(inLock) unlock();
	return lReturn;
}

/*! \brief Wait up to \c inMaxTime seconds to acquire one resource. 
\return True if resource was acquired, false if timed out.

This method decrements a positive semaphore count and returns true. When count is negative or null, it waits up to \c inMaxTime seconds for a resource to become available. If one becomes available before time out, then the method returns true. Otherwise, it returns false. 

If argument \c inLock is true (default), it will first lock the embedded mutex (using the lock method) to protect the internal count. Otherwise, it assumes that the caller has already locked the mutex (using Semaphore::lock) and that it is responsible for unlocking it.

A negative or null time out (default) means that the method should wait indefinitely (for a null time out, use method Semaphore::tryWait).
*/
bool Threading::Semaphore::wait(double inMaxTime, bool inLock)
{
	bool lReturn = true;
	// acquire exclusive access to count
	if(inLock) lock();
	// increment number of waiters
	mWaiters += 1;
	// if ressource is exhausted wait for ressource
	while(mCount <= 0 && lReturn) lReturn = Condition::wait(inMaxTime);
	// decrement count only if condition was signaled (not timed out)
	if(lReturn) mCount -= 1;
	// decrement number of waiters
	mWaiters -= 1;
	// release access to count
	if(inLock) unlock();
	return lReturn;
}
