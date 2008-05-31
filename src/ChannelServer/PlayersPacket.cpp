/*
Copyright (C) 2008 Vana Development Team

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
#include "PacketCreator.h"
#include "Player.h"
#include "Players.h"
#include "PlayersPacket.h"
#include "SendHeader.h"

void PlayersPacket::showMoving(Player* player, vector <Player*> players, unsigned char* packett, int size){
	Packet packet;
	packet.addHeader(SEND_MOVE_PLAYER);
	packet.addInt(player->getPlayerid());
	packet.addInt(0);
	packet.addBuffer(packett+5, size);
	packet.sendTo<Player>(player, players, 0);
}	

void PlayersPacket::faceExperiment(Player* player, vector <Player*> players, int face){
	Packet packet;
	packet.addHeader(SEND_FACE_EXPERIMENT);
	packet.addInt(player->getPlayerid());
	packet.addInt(face);
	packet.sendTo<Player>(player, players, 0);
}

void PlayersPacket::showChat(Player* player, vector <Player*> players, char* msg){
	Packet packet;
	packet.addHeader(SEND_CHAT);
	packet.addInt(player->getPlayerid());
	packet.addByte(player->isGM());
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	packet.sendTo<Player>(player, players, 1);
}

void PlayersPacket::damagePlayer(Player* player, vector <Player*> players, int dmg, int mob){
	Packet packet;
	packet.addHeader(SEND_DAMAGE_PLAYER);
	packet.addInt(player->getPlayerid());
	packet.addByte(-1);
	packet.addInt(dmg);
	packet.addInt(mob);
	packet.addShort(1);
	packet.addByte(0);
	packet.addInt(dmg);
	packet.sendTo<Player>(player, players, 1);
}

void PlayersPacket::showMessage(char* msg, char type){
	Packet packet;
	packet.addHeader(SEND_NOTICE); 
	packet.addByte(type);
	packet.addShort(strlen(msg));
	packet.addString(msg, strlen(msg));
	for (hash_map<int,Player*>::iterator iter = Players::players.begin();
		 iter != Players::players.end(); iter++){
			 packet.send(iter->second);
	}
}

void PlayersPacket::showInfo(Player* player, Player* getinfo){
	Packet packet;
	packet.addHeader(SEND_PLAYER_INFO);
	packet.addInt(getinfo->getPlayerid());
	packet.addByte(getinfo->getLevel());
	packet.addShort(getinfo->getJob());
	packet.addShort(getinfo->getFame());
	packet.addByte(0); // Married
	packet.addShort(1); // Guild Name Len
	packet.addByte(0x2D); // Guild Name
	packet.addShort(0);
	packet.addByte(0);
	packet.send(player);
}

void PlayersPacket::whisperPlayer(Player *target, const string &whisperer_name, int channel, const string &message) {
	Packet packet;
	packet.addHeader(SEND_COMMAND_RESPOND);
	packet.addByte(0x12);
	packet.addString(whisperer_name);
	packet.addShort(channel);
	packet.addString(message);
	packet.send(target);
}

void PlayersPacket::findPlayer(Player* player, const string &name, int map, unsigned char is, bool is_channel){
	Packet packet;
	packet.addHeader(SEND_COMMAND_RESPOND);
	if(map != -1){
		packet.addByte(0x09);
		packet.addString(name);
		if (is_channel)
			packet.addByte(0x03);
		else
			packet.addByte(0x01);
		packet.addInt(map);
		packet.addInt(0);
		packet.addInt(0);
	}
	else {	
		packet.addByte(0x0A);
		packet.addString(name);
		packet.addByte(is);
	}

	packet.send(player);
}

void PlayersPacket::sendToPlayers(unsigned char *data, int len) {
	Packet packet;
	packet.addBuffer(data, len);
	for(hash_map<int,Player*>::iterator iter = Players::players.begin(); iter != Players::players.end(); iter++){
			packet.send(iter->second);
	}
}
