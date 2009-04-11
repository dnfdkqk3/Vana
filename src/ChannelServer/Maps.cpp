/*
Copyright (C) 2008-2009 Vana Development Team

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
#include "Maps.h"
#include "Instance.h"
#include "LuaPortal.h"
#include "MapDataProvider.h"
#include "MapPacket.h"
#include "Pets.h"
#include "Player.h"
#include "PlayerPacket.h"
#include "Players.h"
#include "PacketReader.h"
#include "Summons.h"
#include "WorldServerConnectPacket.h"
#include <sys/stat.h>
#include <string>

using std::string;

Map * Maps::getMap(int32_t mapid) {
	return MapDataProvider::Instance()->getMap(mapid);
}

void Maps::usePortal(Player *player, PortalInfo *portal) {
	if (portal->script.size() != 0) {
		// Scripted portal
		string filename = "scripts/portals/" + portal->script + ".lua";

		struct stat fileInfo;
		if (!stat(filename.c_str(), &fileInfo)) { // Lua Portal script exists
			int32_t map = player->getMap();
			LuaPortal(filename, player->getId(), portal);

			if (map == player->getMap()) {
				// The portal didn't change the map
				MapPacket::portalBlocked(player);
			}
		}
		else {
			string message = "This portal '" + portal->script + "' is currently unavailable.";
			PlayerPacket::showMessage(player, message, 5);
			MapPacket::portalBlocked(player);
		}
	}
	else {
		// Normal portal
		Map *tomap = getMap(portal->tomap);
		if (tomap == 0) {
			string message = "Bzzt. The map you're attempting to travel to doesn't exist.";
			PlayerPacket::showMessage(player, message, 5);
			MapPacket::portalBlocked(player);
			return;
		}
		PortalInfo *nextportal = tomap->getPortal(portal->toname);
		changeMap(player, portal->tomap, nextportal);
	}
}

void Maps::usePortal(Player *player, PacketReader &packet) {
	packet.skipBytes(1);
	if (packet.get<int32_t>() == 0 && player->getHP() == 0) { // Dead
		player->acceptDeath();
		return;
	}
	string portalname = packet.getString();

	Map *tomap = getMap(player->getMap());
	if (tomap == 0)
		return;
	PortalInfo *portal = tomap->getPortal(portalname);
	if (portal == 0) // Exit the function if portal is not found
		return;

	usePortal(player, portal);
}

void Maps::useScriptedPortal(Player *player, PacketReader &packet) {
	packet.skipBytes(1);
	string portalname = packet.getString();

	PortalInfo *portal = getMap(player->getMap())->getPortal(portalname);
	if (portal == 0) // Exit the function if portal is not found
		return;

	usePortal(player, portal);
}

void Maps::changeMap(Player *player, int32_t mapid, PortalInfo *portal) {
	if (!getMap(mapid)) {
		MapPacket::portalBlocked(player);
		return;
	}
	if (portal == 0)
		portal = getMap(mapid)->getSpawnPoint();

	if (player->getInstance() != 0) {
		player->getInstance()->sendMessage(Player_Changemap, mapid, player->getMap());
	}

	getMap(player->getMap())->removePlayer(player);
	player->setMap(mapid);
	player->setMappos(portal->id);
	player->setPos(Pos(portal->pos.x, portal->pos.y - 40));
	player->setStance(0);
	player->setFH(0);
	for (int8_t i = 0; i < 3; i++) {
		if (Pet *pet = player->getPets()->getSummoned(i)) {
			pet->setPos(portal->pos);
		}
	}
	WorldServerConnectPacket::updateMap(ChannelServer::Instance()->getWorldPlayer(), player->getId(), mapid);
	MapPacket::changeMap(player);
	newMap(player, mapid);
}

void Maps::newMap(Player *player, int32_t mapid) {
	Players::Instance()->addPlayer(player);
	getMap(mapid)->addPlayer(player);
	getMap(mapid)->showObjects(player);
	Pets::showPets(player);
	Summons::showSummon(player);
	// Bug in global - would be fixed here:
	// Berserk doesn't display properly when switching maps with it activated - client displays, but no message is sent to any client
	// player->getActiveBuffs()->checkBerserk(true) would override the default of only displaying changes
}
