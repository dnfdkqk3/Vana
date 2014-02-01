/*
Copyright (C) 2008-2014 Vana Development Team

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; version 2
of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#pragma once

#include <memory>

class SessionManager;

// TODO FIXME api
// Remove this class entirely since only Session derives from it and there's no meaningful benefit to this existing
class AbstractSession {
public:
	virtual ~AbstractSession() = default;
	virtual auto disconnect() -> void = 0;
protected:
	AbstractSession(ref_ptr_t<SessionManager> sessionManager, bool encrypted = true) :
		m_sessionManager(sessionManager),
		m_encrypt(encrypted)
	{
	}

	virtual auto start() -> void = 0;
	virtual auto stop() -> void = 0;
	virtual auto handleStart() -> void = 0;
	virtual auto handleStop() -> void = 0;
	auto isEncrypted() const -> bool { return m_encrypt; }

	ref_ptr_t<SessionManager> m_sessionManager;
private:
	friend class SessionManager;

	bool m_encrypt = true;
};