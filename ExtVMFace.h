/*
	This file is part of cpp-ethereum.

	cpp-ethereum is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	cpp-ethereum is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file ExtVMFace.h
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#pragma once

#include <set>
#include <functional>
#include <libdevcore/Common.h>
#include <libevmface/Instruction.h>
#include <libethcore/CommonEth.h>
#include <libethcore/BlockInfo.h>

namespace dev
{
namespace eth
{

struct Post
{
	Address from;
	Address to;
	u256 value;
	bytes data;
	u256 gas;
};

using OnOpFunc = std::function<void(uint64_t /*steps*/, Instruction /*instr*/, bigint /*newMemSize*/, bigint /*gasCost*/, void/*VM*/*, void/*ExtVM*/ const*)>;

/**
 * @brief Interface and null implementation of the class for specifying VM externalities.
 */
class ExtVMFace
{
public:
	/// Null constructor.
	ExtVMFace() = default;

	/// Full constructor.
	ExtVMFace(Address _myAddress, Address _caller, Address _origin, u256 _value, u256 _gasPrice, bytesConstRef _data, bytesConstRef _code, BlockInfo const& _previousBlock, BlockInfo const& _currentBlock, unsigned _depth);

	virtual ~ExtVMFace() = default;

	ExtVMFace(ExtVMFace const&) = delete;
	void operator=(ExtVMFace) = delete;

	/// Read storage location.
	virtual u256 store(u256) { return 0; }

	/// Write a value in storage.
	virtual void setStore(u256, u256) {}

	/// Read address's balance.
	virtual u256 balance(Address) { return 0; }

	/// Read address's code.
	virtual bytes const& codeAt(Address) { return NullBytes; }

	/// Subtract amount from account's balance.
	virtual void subBalance(u256) {}

	/// Determine account's TX count.
	virtual u256 txCount(Address) { return 0; }

	/// Suicide the associated contract and give proceeds to the given address.
	virtual void suicide(Address) { suicides.insert(myAddress); }

	/// Create a new (contract) account.
	virtual h160 create(u256, u256*, bytesConstRef, OnOpFunc const&) { return h160(); }

	/// Make a new message call.
	virtual bool call(Address, u256, bytesConstRef, u256*, bytesRef, OnOpFunc const&, Address, Address) { return false; }

	/// Revert any changes made (by any of the other calls).
	virtual void revert() {}

	/// Get the code at the given location in code ROM.
	byte getCode(u256 _n) const { return _n < code.size() ? code[(size_t)_n] : 0; }

	Address myAddress;			///< Address associated with executing code (a contract, or contract-to-be).
	Address caller;				///< Address which sent the message (either equal to origin or a contract).
	Address origin;				///< Original transactor.
	u256 value;					///< Value (in Wei) that was passed to this address.
	u256 gasPrice;				///< Price of gas (that we already paid).
	bytesConstRef data;			///< Current input data.
	bytesConstRef code;			///< Current code that is executing.
	BlockInfo previousBlock;	///< The previous block's information.
	BlockInfo currentBlock;		///< The current block's information.
	std::set<Address> suicides;	///< Any accounts that have suicided.
	unsigned depth;				///< Depth of the present call.
};

}
}
