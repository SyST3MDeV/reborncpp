// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

#include "include/client_ws.hpp"
#include "include/server_ws.hpp"
#include <future>

#include "SdkHeaders.hpp";
#include <iostream>
#include <fstream>
#include "MinHook.h"
#include "winsock.h";
#include "Hedera.hpp"
#include <chrono>
#include <future>
#include <deque>

using WsServer = SimpleWeb::SocketServer<SimpleWeb::WS>;
using WsClient = SimpleWeb::SocketClient<SimpleWeb::WS>;


size_t base_address = 0;

typedef int(__fastcall* CrashFunc)(UObject* param_1, int* param_2, UObject* param_3, char param_4);

CrashFunc originalCrashFunc = 0;

typedef UObject* (__fastcall* StaticConstructObject)(UClass*, size_t, size_t, size_t, size_t, size_t, size_t, size_t, size_t);

StaticConstructObject origStaticConstructObject = 0;

size_t static_construct_object_param_9 = 0;

/*
unsafe fn fake_notify_accepting_connection(world: usize) -> u8{
	type NotifyAcceptingConnection = unsafe extern "thiscall" fn(world: usize) -> u8;

	let notify_accepting_connection: NotifyAcceptingConnection = unsafe{ std::mem::transmute(orig_world_control_message_recieved)};

	//let mut returnVal: u8 = notify_accepting_connection(world);

	//println!("NotifyAcceptingConnection: {}", returnVal);
	println!("Intercepting NotifyAcceptingConnection! Modifying...");

	let returnVal = 1;

	return returnVal;
}*/

bool listening = false;

template<typename T>
std::vector<T*> getAllObjectsOfType() {
	std::vector<T*> items = std::vector<T*>();

	for (UObject* obj : *GObjects) {
		if (obj && obj->IsA<T>()) {
			items.push_back((T*)obj);
		}
	}

	return items;
}

template<typename T>
T* getLastOfType() {
	return getAllObjectsOfType<T>().back();
}

template<typename T>
void listAllObjectsOfType() {
	std::vector<T*> items = getAllObjectsOfType<T>();

	for (T* item : items) {
		std::cout << std::hex << ((__int64)item) << std::endl;
		std::cout << ((UObject*)item)->GetFullName() << std::endl;
	}
}

UObject* NewObject(UClass* classToInstantiate, UObject* parent) {
	if (static_construct_object_param_9)
		return reinterpret_cast<UObject * (*)(UClass*, UObject*, size_t, size_t, size_t, size_t, size_t, size_t, size_t)>(base_address + 0x008c050)(classToInstantiate, parent, 0, 0, 0, 0, 0, 0, static_construct_object_param_9);
	else {
		std::cout << "Failed to create object: param9 null!" << std::endl;
		return nullptr;
	}
}

void* gbx_malloc(size_t size) {
	int* param_1 = reinterpret_cast<int* (*)()>(base_address + 0x0d33260)();

	return reinterpret_cast<void* (*)(unsigned int param_1, size_t param_2, size_t param_3, size_t param_4, size_t param_5, unsigned int param_6, unsigned int param_7, const char* param_8, unsigned int param_9,const char* param_10)>(base_address + 0x0d2e160)(*(int*)(param_1 + 0x10), 0, size, 0x8, 0, 0, 0x31c0019,
		"t:\\POPLAR-PATCH-PC\\Development\\Src\\Core\\Src\\gbxmem.cpp", 0x46, "appMalloc");
	/*  lVar1 = FUN_140d33260();
  FUN_140d2e160(*(undefined4 *)(lVar1 + 0x10),0,param_2,0x10,0,0,0x31c0019,
                "t:\\POPLAR-PATCH-PC\\Development\\Src\\Core\\Src\\gbxmem.cpp",0x46,"appMalloc");*/
}

void FlushNet(UNetConnection* connection, bool bIgnoreSimulation) {
	// was 0x0163410
	//now 061b9e0
	reinterpret_cast<void (*)(UNetConnection * connection, bool bIgnoreSimulation)>(base_address + 0x061b9e0)(connection, bIgnoreSimulation);
}

struct FOutBunch {
	char UnknownData[0x50]; //0x0
	TArray<uint8_t> Data; //0x50
	int Num; //0x60
	int Max; //0x64
	FOutBunch* Next; //0x68
	char UnknownData01[0x8]; //0x70
	double Time; //0x78
	signed int RecievedAck; //0x80
	int ChIndex; //0x84
	int ChType; //0x88
	int ChSequence; //0x8c
	int PacketId; //0x90
	char bOpen; //0x94
	char bClose; //0x95
	char bReliable; //0x96
};

FOutBunch ConstructChallengeBunch(UTcpipConnection* connection, UControlChannel* controlChannel) {
	FOutBunch returnVal = FOutBunch();

	returnVal.bReliable = true;
	returnVal.bClose = false;
	returnVal.ChIndex = controlChannel->ChIndex;
	returnVal.ChType = controlChannel->ChType;

	//0x21AF - af 21

	returnVal.Data = TArray<uint8_t>();

	for (int i = 0; i < ((512 * 8) - 64 - 1 - 16); i++) {
		returnVal.Data.push_back(0x0);
	}

	returnVal.Data[0] = 0x3; //0x3 = Challenge bunch type
	returnVal.Data[1] = 0xAF; //Version
	returnVal.Data[2] = 0x21; //Version
	
	//returnVal.Data.push_back(0x00); //Challenge String

	returnVal.Num = returnVal.Data.size();
	returnVal.Max = returnVal.Data.size();

	return returnVal;
}

int ControlChannelSendBunch(UControlChannel* channel, FOutBunch* bunch, bool merge) {
	return reinterpret_cast<int (*)(UControlChannel * channel, FOutBunch * bunch, bool merge)>(base_address + 0x0610cf0)(channel, bunch, merge);
}

typedef uint8_t(__cdecl* NotifyAcceptingConnection)();

NotifyAcceptingConnection origNotifyAcceptingConnection = 0;

//0x045adb0
uint8_t NotifyAcceptingConnectionHook() { //uint8_t
	std::cout << "Forcing accept connection" << std::endl;

	return 1;
}

typedef size_t(__thiscall* ForceToZero)();

ForceToZero origForceToZero = 0;

//0x0209210
size_t ForceToZeroHook() {
	std::cout << "Forcing to zero!" << std::endl;

	return 0;
}

int CrashFuncHook(UObject* param_1, int* param_2, UObject* param_3, char param_4) {
	//std::cout << param_1->GetFullName() << std::endl;
	//std::cout << param_3->GetFullName() << std::endl;

	//std::cout << "thing time!" << std::endl;

	return originalCrashFunc(param_1, param_2, param_3, param_4);
	//return param_2[0x1a];
	//return 0;
}

UObject* StaticConstructObjectHook(UClass* param1, size_t param2, size_t param3, size_t param4, size_t param5, size_t param6, size_t param7, size_t param8, size_t param9) {
	if(param9)
		static_construct_object_param_9 = param9;

	return origStaticConstructObject(param1, param2, param3, param4, param5, param6, param7, param8, param9);
}

typedef void(__thiscall* WorldControlMessage)(UWorld* world, UNetConnection* connection, uint8_t message, void* inbunch);

WorldControlMessage origWorldControlMessage = 0;

UFunction* clientHasLoadedWorldFunction = 0;

std::vector<UNetConnection*> connections = std::vector<UNetConnection*>();

void WorldControlMessageHook(UWorld* world, UNetConnection* connection, uint8_t message, void* inbunch) {
	origWorldControlMessage(world, connection, message, inbunch);
	std::cout << "Message Recieved!" << std::endl;
	std::cout << (int)message << std::endl;

	if (message == (uint8_t)0) {
		std::cout << "Welcoming client!" << std::endl;
		reinterpret_cast<void* (*)(UWorld*, UNetConnection*)>(base_address + 0x045b060)(world, connection);

		APoplarAccessControl* ac = getLastOfType<APoplarAccessControl>();

		FString options = FString();
		FString address = FString();
		
		FUniqueNetId netID = FUniqueNetId();

		netID.bHasValue = true;
		netID.RawId[0] = 0x1;

		FString errorMessage = FString();

		ac->eventPreLogin(options, address, netID, false, errorMessage);

		std::cout << "PreLogin err: " << errorMessage.ToString() << std::endl;

		//0x045b060
		/*
		std::cout << "Challenging client!" << std::endl;

		UControlChannel* controlChannel = getLastOfType<UControlChannel>();

		FOutBunch bunch = ConstructChallengeBunch((UTcpipConnection*)connection, controlChannel);

		ControlChannelSendBunch(controlChannel, &bunch, false);

		FlushNet(connection, true);
		*/
	}
	else if (message == (uint8_t)9) {
		std::cout << "Joining Client..." << std::endl;

		for (APlayerController* pc : getAllObjectsOfType<APlayerController>()) {
			if (pc->GetFullName().find("Default") == std::string::npos) {
				std::cout << pc << std::endl;
				std::cout << pc->GetFullName() << std::endl;
				std::cout << pc->PlayerReplicationInfo->PlayerName.ToString() << std::endl;
				std::cout << (int)pc->NetPlayerIndex << std::endl;
			}
		}
		
		//ulonglong UWorldSpawnPlayActor(undefined8 param_1, undefined8 param_2, undefined8 param_3, longlong param_4, undefined8 param_5, undefined8 param_6, undefined param_7)
		//APlayerController* UWorld::SpawnPlayActor(UPlayer* Player, ENetRole RemoteRole, const FURL& InURL, const TSharedPtr<FUniqueNetId>& UniqueId, FString& Error, uint8 InNetPlayerIndex)
		//UWorld::SpawnPlayActor(UWorld * this, UPlayer * param_1, ENetRole param_2, FURL * param_3, FUniqueNetId * param_4, FString * param_5, uchar param_6)
		UWorld* theWorld = UObject::FindObject<UWorld>("World Wishbone_P.TheWorld");
		FURL theURL = FURL();

		FUniqueNetId netID = FUniqueNetId();

		netID.bHasValue = true;
		netID.RawId[0] = 0x1;

		FString err = FString();

		reinterpret_cast<void(__thiscall*)(UWorld* world, UPlayer* player, ENetRole RemoteRole, FURL* url, FUniqueNetId* netID, FString* err, uint8_t InNetPlayerIndex)>(base_address + 0x03ef7b0)(theWorld, connection, ENetRole::ROLE_AutonomousProxy, &theURL, &netID, &err, 0);

		APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();

		connection->Actor = pc;

		pc->RemoteRole = ENetRole::ROLE_AutonomousProxy;
		//pc->MyPoplarPRI->SetPlayerTeam(getAllObjectsOfType<ATeamInfo>()[4]);
		
		//pc->SwitchToPendingPlayerClass();
		//pc->PlayerReplicationInfo->bOnlySpectator = false;
		//pc->PlayerReplicationInfo->bIsSpectator = false;
		//pc->PoplarPSI->bCurrentlySpectating = false;
		//pc->ServerRestartPlayer();

		connections.push_back(connection);

		listening = true;

		/*
		FString portal = FString();
		FString options = FString();

		FUniqueNetId netID = FUniqueNetId();

		netID.bHasValue = true;
		netID.RawId[0] = 0x1;

		FString err = FString();

		AGameInfo* gameInfo = getLastOfType<AGameInfo>();

		gameInfo->bDelayedStart = false;

		APlayerController* pc = gameInfo->eventLogin(portal, options, netID, err);

		pc->NetPlayerIndex = 0;
		pc->Player = connection;
		connection->Actor = pc;

		pc->Role = ENetRole::ROLE_Authority;
		pc->RemoteRole = ENetRole::ROLE_AutonomousProxy;

		//gameInfo->bDelayedStart = false;

		gameInfo->eventPostLogin(pc);

		listening = true;

		//gameInfo->eventRestartPlayer(pc);
		*/

		/*
		std::cout << pc->PlayerReplicationInfo->bOnlySpectator << std::endl;

		std::cout << pc->CanRestartPlayer() << std::endl;

		std::cout << pc->Pawn << std::endl;

		std::cout << pc->PlayerReplicationInfo << std::endl;
		*/
	}
	else if (message == (uint8_t)15) {
		APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();

		//pc->MyPoplarPRI->SetPlayerTeam(getAllObjectsOfType<ATeamInfo>()[3]);

		//pc->RemoteRole = ENetRole::ROLE_AutonomousProxy;

		//std::cout << pc->IsSpectating() << std::endl;
		//std::cout << pc->CanRestartPlayer() << std::endl;

		
		
		//pc->ClientGotoState(FName(1830), FName(1830));
		//pc->GotoState(FName(1830));

		

		/*[PE] PoplarPlayerController Wishbone_P.TheWorld.PersistentLevel.PoplarPlayerController: Function PoplarGame.PoplarPlayerController.AfterReceivedPlayerWaitForCharacterLoad
[PE] PoplarPlayerReplicationInfo Wishbone_P.TheWorld.PersistentLevel.PoplarPlayerReplicationInfo: Function WillowGame.WillowPlayerReplicationInfo.RunOnceClientInitialize*/

		//pc->AfterReceivedPlayerWaitForCharacterLoad();
		//pc->MyPoplarPRI->RunOnceClientInitialize();

		//pc->ServerChangeTeam(1);
		//pc->ServerSetPlayerName(FString((const char*)L"not gwog"));

		//pc->MyPoplarPRI->SetPlayerTeam(getAllObjectsOfType<ATeamInfo>()[1]);
		//pc->MyPoplarPRI->bIsSpectator = false;
		//pc->MyPoplarPRI->bOnlySpectator = false;
		//pc->MyPoplarPRI->respawn

		//pc->GotoState(FName("PlayerWalking"));
		//pc->ClientGotoState(FName("PlayerWalking"), FName("PlayerWalking"));
		//pc->MyPoplarPRI->ServerAdvanceCurrentPlayerState(EPlayerState::PlayerState_Playing);
		//getLastOfType<APlayerController>()->Destroy();
		//return PlayerState && !PlayerState->bOnlySpectator && HasClientLoadedCurrentWorld() && PendingSwapConnection == NULL;

		/*
		for (APlayerController* pc : getAllObjectsOfType<APlayerController>()) {
			if (pc->GetFullName().find("Default") == std::string::npos) {
				std::cout << pc << std::endl;
				std::cout << pc->GetFullName() << std::endl;
				std::cout << pc->PlayerReplicationInfo->PlayerName.ToString() << std::endl;
				std::cout << pc->PendingSwapConnection << std::endl;
				std::cout << pc->CanRestartPlayer() << std::endl;
				std::cout << getLastOfType<APoplarGameInfo>()->PlayerCanRestart(pc) << std::endl;
				pc->ServerRestartPlayer();
			}
		}
		*/

		
		/*
		APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();

		pc->SwitchToPendingPlayerClass();
		pc->PlayerReplicationInfo->bOnlySpectator = false;
		pc->PlayerReplicationInfo->bIsSpectator = false;
		//pc->eventServerSendInitialClientPerformanceData(FClientInitialPerformanceData());

		//APoplarGameInfo* gi = getLastOfType<APoplarGameInfo>();

		pc->Player = connection;
		//pc->

		//pc->PlayerReplicationInfo->SetPlayerTeam(getAllObjectsOfType<ATeamInfo>()[2]);
		
		//std::cout << pc->CanRestartPlayer() << std::endl;

		std::cout << pc->IsSpectating() << std::endl;
		std::cout << pc->CanRestartPlayer() << std::endl;
		
		pc->ServerRestartPlayer();
		*/

		/*
		FString empty = FString();

		AActor* playerStart = gi->FindPlayerStart(pc, 1, empty);
		UClass* defaultClass = gi->GetDefaultPlayerClass(pc);

		pc->PlayerReplicationInfo->bHasBeenWelcomed = true;

		pc->Pawn = (APawn*)gi->Spawn(defaultClass, nullptr, FName(), playerStart->Location, playerStart->Rotation, nullptr, true);
		pc->Pawn->Controller = pc;
		pc->Pawn->PossessedBy(pc, false);
		pc->SetRotation(pc->Pawn->Rotation);
		//pc->GivePawn(pc->Pawn);

		pc->PlayerReplicationInfo->bOnlySpectator = false;
		pc->PoplarPSI->PoplarPawn = (APoplarPawn*)pc->Pawn;
		pc->PoplarPSI->Pawn = (APoplarPawn*)pc->Pawn;

		pc->Pawn->eventPreBeginPlay();
		pc->Pawn->eventPostBeginPlay();

		std::cout << pc->MyPoplarPRI << std::endl;
		std::cout << pc->PoplarPSI << std::endl;
		std::cout << pc->Pawn << std::endl;
		*/
		//gi->eventRestartPlayer(pc);

		//reinterpret_cast<void(__thiscall*)(UWorld * world, AActor * actor, bool bNetForce)>(base_address + 0x03ef070)(UObject::FindObject<UWorld>("World Wishbone_P.TheWorld"), getLastOfType<APlayerController>(), true);

		//listAllObjectsOfType<APlayerController>();
	}
}

//void UNetPendingLevelNotifyControlMessage(longlong param_1, longlong param_2, undefined param_3, longlong* param_4)
//04c2000
//void UPendingNetGame::NotifyControlMessage(UNetConnection* Connection, uint8 MessageType, class FInBunch& Bunch)

typedef void(__thiscall* PendingNetLevelNotifyControlMessage)(UNetPendingLevel* PendingLevel, UNetConnection* Connection, uint8_t MessageType, void* FInBunch);

PendingNetLevelNotifyControlMessage origPendingNetLevelNotifyControlMessage = 0;

void PendingNetLevelNotifyControlMessageHook(UNetPendingLevel* PendingLevel, UNetConnection* Connection, uint8_t MessageType, void* FInBunch) {
	origPendingNetLevelNotifyControlMessage(PendingLevel, Connection, MessageType, FInBunch);

	std::cout << "Recieved Pending Net Level Control Message" << std::endl;

	std::cout << (int)MessageType << std::endl;
}

//reinterpret_cast<void* (*)(size_t, const wchar_t*, size_t)>(base_address + 0x01fca00)((__int64)((*GObjects)[0]) + 0x25ebde8, command, 0);

typedef void(__thiscall* EngineExecType)(size_t, const wchar_t*, size_t);

EngineExecType origEngineExec = 0;

size_t global_foutputdevice = 0;

void EngineExecHook(size_t param1, const wchar_t* command, size_t foutputdevice) {
	if (global_foutputdevice == 0) {
		std::cout << "yoinked foutputdevice!" << std::endl;
		global_foutputdevice = foutputdevice;
	}

	origEngineExec(param1, command, foutputdevice);
}

//longlong ** FUN_140c546b0(longlong param_1,longlong **param_2)

typedef void(__thiscall* NewCrashFunc)(UObject* param_1, UObject* param_2, UObject* param_3, UObject* param_4, UObject* param_5);

NewCrashFunc origNewCrashFunc = 0;

/*void FUN_140c73b80(longlong param_1,longlong param_2,ulonglong param_3,longlong param_4,
                  undefined *param_5)*/
void NewCrashFuncHook(UObject* param_1, UObject* param_2, UObject* param_3, UObject* param_4, UObject* param_5) {
	std::cout << std::hex << (__int64)param_4 << std::endl;

	std::cout << param_4->GetFullName() << std::endl;

	return origNewCrashFunc(param_1, param_2, param_3, param_4, param_5);
}

//0x109ca0


typedef void*(__thiscall* ProcessEvent)(UObject* object, UFunction* function, void* params);

ProcessEvent origProcessEvent = 0;

bool bStartPELogging = false;

bool bProcInGameThread = false;

void* ProcessEventHook(UObject* object, UFunction* function, void* params) {
	if (bStartPELogging && object && function) {
		std::cout << "[PE] " << object->GetFullName() << ": " << function->GetFullName() << std::endl;
	}
	//UpdateSpawnBots

	return origProcessEvent(object, function, params);
}

std::vector<UTcpipConnection*> getClientConnections() {
	std::vector<UTcpipConnection*> toReturn = getAllObjectsOfType<UTcpipConnection>();

	toReturn.erase(toReturn.begin());

	return toReturn;
}

std::vector<UActorChannel*> getActorChannels() {
	std::vector<UActorChannel*> toReturn = getAllObjectsOfType<UActorChannel>();

	toReturn.erase(toReturn.begin());

	return toReturn;
}

std::vector<AActor*> getActors() {
	std::vector<AActor*> toReturn = getAllObjectsOfType<AActor>();//getAllObjectsOfType<AActor>();// //getAllObjectsOfType<AActor>();
	
	/*
	if (replicationFrame == 0) {
		toReturn = getAllObjectsOfType<AActor>();
	}

	if (replicationFrame == 1)
		for (APoplarMetaPlayerReplicationInfo* pc : getAllObjectsOfType<APoplarMetaPlayerReplicationInfo>())
			toReturn.push_back(pc);

	if (replicationFrame == 2)
		for (APoplarPlayerStateInfo* pc : getAllObjectsOfType<APoplarPlayerStateInfo>())
			toReturn.push_back(pc);

	if (replicationFrame == 3)
		for (APoplarStatReplicationInfo* pc : getAllObjectsOfType<APoplarStatReplicationInfo>())
			toReturn.push_back(pc);

	if (replicationFrame == 4)
		for (APoplarPlayerReplicationInfo* pc : getAllObjectsOfType<APoplarPlayerReplicationInfo>())
			toReturn.push_back(pc);

	if (replicationFrame == 5)
		for (APoplarInteractiveObject* pc : getAllObjectsOfType<APoplarInteractiveObject>())
			toReturn.push_back(pc);

	if (replicationFrame == 6)
		for (APoplarPlayerController* pc : getAllObjectsOfType<APoplarPlayerController>())
			toReturn.push_back(pc);

	if (replicationFrame == 7)
		for (APoplarWeapon* pc : getAllObjectsOfType<APoplarWeapon>())
			toReturn.push_back(pc);

	if (replicationFrame == 8)
		for (APawn* pc : getAllObjectsOfType<APawn>())
			toReturn.push_back(pc);

	if (replicationFrame == 9)
	for (AWorldInfo* pc : getAllObjectsOfType<AWorldInfo>())
		toReturn.push_back(pc);

	if (replicationFrame == 10)
		for (AGameReplicationInfo* pc : getAllObjectsOfType<AGameReplicationInfo>())
			toReturn.push_back(pc);

	if (replicationFrame == 11)
		for (APoplarTeamInfo* pc : getAllObjectsOfType<APoplarTeamInfo>())
			toReturn.push_back(pc);

	if (replicationFrame == 12)
		for (AResourcePoolManager* pc : getAllObjectsOfType<AResourcePoolManager>())
			toReturn.push_back(pc);

	if (replicationFrame == 13)
		for (APoplarInventoryManager* pc : getAllObjectsOfType<APoplarInventoryManager>())
			toReturn.push_back(pc);

	replicationFrame++;

	if (replicationFrame > 13) {
	if (replicationFrame > 13) {
		replicationFrame = 0;
	}

	//PoplarInventoryManager 

	//toReturn.erase(toReturn.begin());
	*/

	return toReturn;
}

typedef void(__thiscall* GameEngineTick)(UGameEngine* engine, float deltaTime);

GameEngineTick origGameEngineTick = 0;

UActorChannel* getChannel(AActor* actor, std::vector<UActorChannel*> actorChannels) {
	for (UActorChannel* actorToCompare : actorChannels) {
		if (actor == actorToCompare->Actor) {
			return actorToCompare;
		}
	}

	return nullptr;
}

int numTicksDelayed = 0;

std::vector<UActorChannel*> actorChannels = std::vector<UActorChannel*>();

std::vector< std::shared_ptr<WsServer::Connection>> wsconnections = std::vector< std::shared_ptr<WsServer::Connection>>();
std::vector<bool> wsconnectionAckStatus = std::vector<bool>();
std::deque<HederaDefinitions::ReplicationFrame> replicationFrameQueue = std::deque< HederaDefinitions::ReplicationFrame>();
std::shared_ptr<WsClient::Connection> clientWsConnection;

bool isClientAndConnected = false;

void GameEngineTickHook(UGameEngine* engine, float deltaTime) {
	origGameEngineTick(engine, deltaTime);

	if (isClientAndConnected) {
		if (!replicationFrameQueue.empty()) {
			HederaDefinitions::ReplicationFrame inRepFrame = replicationFrameQueue[0];

			for (int i = 0; i < inRepFrame.actorreplicationmessages_size(); i++) {
				HederaDefinitions::ActorReplicationMessage repMsg = inRepFrame.actorreplicationmessages()[i];

				actorChannels = getActorChannels();

				if (repMsg.actortype() == HederaDefinitions::ActorReplicationMessage::ActorType::ActorReplicationMessage_ActorType_ACTOR_TYPE_POPLAR_PLAYER_REPLICATION_INFO) {
					bool bFound = false;
					for (UActorChannel* channel : actorChannels) {
						if (channel->ChIndex == repMsg.actorid()) {
							bFound = true;

							HederaDefinitions::APoplarPlayerReplicationInfo* parsedPPRI = new HederaDefinitions::APoplarPlayerReplicationInfo();

							parsedPPRI->ParseFromString(repMsg.replicationmessage());

							APoplarPlayerReplicationInfo* PPRI = (APoplarPlayerReplicationInfo*)channel->Actor;

							if (PPRI) {
								Hedera::Parser::ParseAPoplarPlayerReplicationInfo(*parsedPPRI, PPRI, actorChannels, GObjects);
							}
							else {
								std::cout << "[HEDERA] Replication failed, actor in channel is null!" << std::endl;
							}
						}
					}

					if (!bFound) {
						std::cout << "[HEDERA] Replication failed, couldn't find an actor channel with the correct ID!" << std::endl;
					}
				}
			}

			replicationFrameQueue.pop_back();

			clientWsConnection.get()->send("");
		}
	}

	if (listening) {
		std::vector<AActor*> actors = getActors();

		if (connections.size() > 0) {
			//std::vector< UPackageMap*> packageMaps = getAllObjectsOfType<UPackageMap>();

			/*
			if (!packageFuckeryComplete) {
				packageFuckeryComplete = true;

				for (UPackage* package : getAllObjectsOfType<UPackage>()) {
					std::cout << "package netindex: " << package->NetIndex << std::endl;
					if (package->NetIndex == -1) {
						package->NetIndex = 1;
					}
					//reinterpret_cast<void (*)(UPackageMap*, UPackage*)>(base_address + 0x001ece0)(packageMaps[packageMaps.size() - 1], package);
				}

				//reinterpret_cast<void (*)(UPackageMap*)>(base_address + 0x001eb40)(packageMaps[packageMaps.size() - 1]);
				//reinterpret_cast<void (*)(UPackageMap*)>(base_address + 0x001e8d0)(packageMaps[packageMaps.size() - 1]);
				std::cout << "fucking package..." << std::endl;
				reinterpret_cast<void (*)(UPackageMap*)>(base_address + 0x001e8d0)(packageMaps[packageMaps.size() - 1]);
			}
		*/

		//reinterpret_cast<void (*)(UPackageMap*, UPackageMap*)>(base_address + 0x001e080)(packageMaps[packageMaps.size() - 1], packageMaps[packageMaps.size() - 2]);
		//reinterpret_cast<void (*)(UPackageMap*)>(base_address + 0x001eb40)(packageMaps[packageMaps.size() - 1]);
		//reinterpret_cast<void (*)(UPackageMap*)>(base_address + 0x001e8d0)(packageMaps[packageMaps.size() - 1]);

		//Mark 
		/*
		for (UActorChannel* actorChannel : actorChannels) {
			if (actorChannel->Actor && actorChannel->Actor->bNetDirty && actorChannel->Actor->RemoteRole != ENetRole::ROLE_None) {
				actorChannel->ActorDirty = true; //GUESSED, CHECK IF PROBLEMS
				actorChannel->Actor->bNetDirty = false;
			}
		}
		*/

			for (UNetConnection* connection : connections) {
				int actorIDX = 0;

				HederaDefinitions::ReplicationFrame* replicationFrame = new HederaDefinitions::ReplicationFrame();

				for(int i = actors.size() - 1; i >= 0; i--) { //(AActor* actor : actors)
					AActor* actor = actors[i];
					if (actor->RemoteRole != ENetRole::ROLE_None && actor->GetFullName().find("Default__") == std::string::npos) {
						if (actor->IsA<APlayerController>() && (connection->Actor != actor)) {
							continue;
						}

						//if ((actor->IsA<APlayerController>() || actor->IsA<APawn>()) && !actor->Owner) {
							//std::cout << "associating owner" << std::endl;
							//actor->bNetOwner = true;
						//}

						UActorChannel* channel = getChannel(actor, actorChannels);

						//__int64 gra = reinterpret_cast<__int64(__thiscall*)(UPackageMap*, UClass*)>(base_address + 0x001e310)(packageMaps.back(), actor->Class);

						//001e310, 2 pointers, first is package

						bool bInitial = false;

						if (!channel) {// && gra != 0) {
							//std::cout << actor->GetFullName() << std::endl;
							channel = reinterpret_cast<UActorChannel * (__thiscall*)(UNetConnection * connection, int channelType, uint32_t openedLocally, int chIndex)>(base_address + 0x061daa0)(connection, 2, 1, -1);

							//std::cout << std::hex << channel << std::endl;

							actorChannels.push_back(channel);
							//0x0611970
							//void UActorChannel::SetChannelActor( AActor* InActor )
							reinterpret_cast<void(__thiscall*)(UActorChannel*, AActor*)>(base_address + 0x0611970)(channel, actor);
							
							//channel->Actor = actor;
							//channel->ActorClass = actor->Class;

							//actor->bSkipActorPropertyReplication = false;
							bInitial = true;
						}
						/*
							else if (gra == 0) {
								std::cout << "Couldn't find " << actor->Class->GetFullName() << std::endl;
							}
							*/

						if (channel && channel->Actor) {
							//std::cout << "[REPL] " << channel->Actor->GetFullName() << std::endl;

							if (!channel->Actor->WorldInfo) {
								continue;
								//std::cout << "invalid world info, associating..." << std::endl;

								//channel->Actor->WorldInfo = UObject::FindObject<AWorldInfo>("WorldInfo Wishbone_P.TheWorld.PersistentLevel.WorldInfo");
							}

							channel->Connection = connection;

							//actor->bNetDirty = false;
							//channel->ActorDirty = false;
							/*
							if (actor->IsA(APoplarPlayerController::StaticClass())) {
								actor->bSkipActorPropertyReplication = true;

								auto dumpTimeStart = std::chrono::high_resolution_clock::now();

								HederaDefinitions::APoplarPlayerController* dumpedPC = Hedera::Dumper::DumpAPoplarPlayerController((APoplarPlayerController*)actor, actorChannels);

								std::string out;

								dumpedPC->SerializeToString(&out);

								auto dumpTimeEnd = std::chrono::high_resolution_clock::now();

								std::cout << "Dump: " << (std::chrono::duration_cast<std::chrono::milliseconds>(dumpTimeEnd - dumpTimeStart)).count() << "ms" << std::endl;

								auto parseTimeStart = std::chrono::high_resolution_clock::now();

								Hedera::Parser::ParseAPoplarPlayerController(*dumpedPC, (APoplarPlayerController*)actor, actorChannels, GObjects);

								auto parseTimeEnd = std::chrono::high_resolution_clock::now();

								std::cout << "Parse: " << (std::chrono::duration_cast<std::chrono::milliseconds>(parseTimeEnd - parseTimeStart)).count() << "ms" << std::endl;
							}
							*/
							
							//actor->bForceNetUpdate = true;
							//actor->bPendingNetUpdate = true;
							//actor->bSkipActorPropertyReplication = false;
							//actor->bSkipActorPropertyReplication = false;
							//actor->bSkipHiddenReplication = false;
							//actor->bSkipRotationReplication = false;
							//actor->bSkipStillDirtyReplication = false;
							//actor->bForceRotationReplication = true;
							//actor->bOnlyDirtyReplication = false;
							//actor->bNetInitial = true;
							//actor->bNetDirty = true;
							
							//UNetConnection

							//if (!reinterpret_cast<bool (*)(UNetConnection * connection, uint8_t saturate)>(base_address + 0x0613050)(connection, 1)) {
								//reinterpret_cast<void (*)(UNetConnection * connection, uint8_t bIgnoreSimulation)>(base_address + 0x0163410)(connection, 0);
								//std::cout << "saturated, flushing net!" << std::endl;
								//std::cout << "SATURATED! stopping replication" << std::endl;
								//break;
							//}

							//else {

							//}

							//std::cout << actor->GetFullName() << std::endl;

							*(unsigned int*)(base_address + 0x357aed8) = 0;

							//(*(uint *)(param_1 + 0x13c) & 0x100000) != 0)

							
							
							/*
							if (actor->IsA(APoplarPlayerController::StaticClass())) {
								HederaDefinitions::APoplarPlayerController* pb_pc = Hedera::Dumper::DumpAPoplarPlayerController((APoplarPlayerController*)actor, actorChannels);

								for (auto connection : wsconnections) {
									connection.get()->send(pb_pc->SerializeAsString());
								}
							}
							*/

							if (actor->IsA(APoplarPlayerReplicationInfo::StaticClass()) && !bInitial) {
								HederaDefinitions::ActorReplicationMessage* msg = replicationFrame->add_actorreplicationmessages();
								msg->set_actortype(HederaDefinitions::ActorReplicationMessage::ActorType::ActorReplicationMessage_ActorType_ACTOR_TYPE_POPLAR_PLAYER_REPLICATION_INFO);
								msg->set_isinitial(false);
								msg->set_actorid(channel->ChIndex);
								std::string serializedMsg = Hedera::Dumper::DumpAPoplarPlayerReplicationInfo((APoplarPlayerReplicationInfo*)actor, actorChannels)->SerializeAsString();
								msg->set_replicationmessage(serializedMsg);
							}
							else {
								reinterpret_cast<void (*)(UActorChannel* channel)>(base_address + 0x0613050)(channel);
							}

							//}
							//else {
								//std::cout << "SATURATED! not replicating...." << std::endl;
							//}

							//std::cout << channel << std::endl;

							//if (actor->bNetDirty)
								//actor->bNetDirty = false;
						}
						else {
							std::cout << actor->GetFullName() << std::endl;
							std::cout << "replication failed!" << std::endl;
						}
						//0x0613050


					//}
						actorIDX++;
					}
				}

				//0x061daa0 - CreateChannel(UNetConnection* connection, EChannelType ChType (2 is actor), uint32_t openedLocally, int chIndex(-1))
				for (int i = 0; i < wsconnections.size(); i++) {
					if (wsconnectionAckStatus[i]) {
						wsconnections[i].get()->send(replicationFrame->SerializeAsString());
						wsconnectionAckStatus[i] = false;
					}
				}
			}

			//std::cout << "Replicated " << numActors << " actors!" << std::endl;

			/*
			for (UActorChannel* actorChannel : actorChannels) {
				//0x0614600

				reinterpret_cast<void (*)(UActorChannel * channel)>(base_address + 0x0614600)(actorChannel);
			}
			*/
		}

		for (UNetConnection* connection : connections) {
			//061dc00
			reinterpret_cast<void (*)(UNetConnection* connection)>(base_address + 0x061dc00)(connection);
		}
	}
}

typedef void* (__thiscall* ProcessRemoteFunction)(AActor* actor, UFunction* function, void* params, void* stack);

ProcessRemoteFunction origProcessRemoteFunction = 0;

void* ProcessRemoteFunctionHook(AActor* actor, UFunction* function, void* params, void* stack) {
	if (!actor->WorldInfo) {
		actor->WorldInfo = UObject::FindObject<AWorldInfo>("WorldInfo Wishbone_P.TheWorld.PersistentLevel.WorldInfo");
	}

	return origProcessRemoteFunction(actor, function, params, stack);
}

//UBOOL AActor::ProcessRemoteFunction( UFunction* Function, void* Parms, FFrame* Stack )
//0x0728fd0

typedef uint8_t(__thiscall* AlwaysReturnOne)(void* param1, void* param2, void* param3, void* param4);

AlwaysReturnOne origAlwaysReturnOne = 0;

uint8_t AlwaysReturnOneHook(void* param1, void* param2, void* param3, void* param4) {
	return 1;
}

//byte FUN_1403d79e0(longlong param_1,longlong *param_2,undefined8 param_3,longlong *param_4)
//stub functionality - always return 1

void initHooks() {
	MH_Initialize();

	StaticConstructObject staticConstructObject = (StaticConstructObject)(base_address + 0x008c050);

	MH_CreateHook(staticConstructObject, StaticConstructObjectHook, reinterpret_cast<LPVOID*>(&origStaticConstructObject));

	MH_EnableHook(staticConstructObject);

	CrashFunc crashFunc = (CrashFunc)(base_address + 0x0d2d560);

	MH_CreateHook(crashFunc, CrashFuncHook, reinterpret_cast<LPVOID*>(&originalCrashFunc));

	MH_EnableHook(crashFunc);

	NotifyAcceptingConnection notifyAcceptingConnection = (NotifyAcceptingConnection)(base_address + 0x045adb0);

	MH_CreateHook(notifyAcceptingConnection, NotifyAcceptingConnectionHook, reinterpret_cast<LPVOID*>(&origNotifyAcceptingConnection));

	MH_EnableHook(notifyAcceptingConnection);

	ForceToZero forceToZero = (ForceToZero)(base_address + 0x0209210);

	MH_CreateHook(forceToZero, ForceToZeroHook, reinterpret_cast<LPVOID*>(&origForceToZero));

	//MH_EnableHook(forceToZero);

	WorldControlMessage worldControlMessage = (WorldControlMessage)(base_address + 0x045c540);

	MH_CreateHook(worldControlMessage, WorldControlMessageHook, reinterpret_cast<LPVOID*>(&origWorldControlMessage));

	MH_EnableHook(worldControlMessage);

	EngineExecType engineExecThing = (EngineExecType)(base_address + 0x01fca00);

	MH_CreateHook(engineExecThing, EngineExecHook, reinterpret_cast<LPVOID*>(&origEngineExec));

	MH_EnableHook(engineExecThing);

	NewCrashFunc newCrashFunc = (NewCrashFunc)(base_address + 0x0c73b80);

	MH_CreateHook(newCrashFunc, NewCrashFuncHook, reinterpret_cast<LPVOID*>(&origNewCrashFunc));

	//MH_EnableHook(newCrashFunc);

	ProcessEvent processEvent = (ProcessEvent)(base_address + 0x109ca0);

	MH_CreateHook(processEvent, ProcessEventHook, reinterpret_cast<LPVOID*>(&origProcessEvent));

	MH_EnableHook(processEvent);

	GameEngineTick gameEngineTick = (GameEngineTick)(base_address + 0x0207e10);

	MH_CreateHook(gameEngineTick, GameEngineTickHook, reinterpret_cast<LPVOID*>(&origGameEngineTick));

	MH_EnableHook(gameEngineTick);

	ProcessRemoteFunction processRemoteFunction = (ProcessRemoteFunction)(base_address + 0x0728fd0);

	MH_CreateHook(processRemoteFunction, ProcessRemoteFunctionHook, reinterpret_cast<LPVOID*>(&origProcessRemoteFunction));

	MH_EnableHook(processRemoteFunction);

	AlwaysReturnOne alwaysReturnOne = (AlwaysReturnOne)(base_address + 0x03d79e0);

	MH_CreateHook(alwaysReturnOne, AlwaysReturnOneHook, reinterpret_cast<LPVOID*>(&origAlwaysReturnOne));

	//MH_EnableHook(alwaysReturnOne);

	//0x109ca0

	//FUN_140c546b0

	/*
	typedef size_t(__thiscall* ForceToZero)();

ForceToZero origForceToZero = 0;

//0x0209210*/

	//0d2d560
}

void init() {
	base_address = (size_t)GetModuleHandleA("Battleborn.exe");

	GObjects = reinterpret_cast<TArray<UObject*>*>(base_address + (size_t)0x035152D0);
	GNames = reinterpret_cast<TArray<FNameEntry*>*>(base_address + (size_t)0x03515228);

	AllocConsole();
	//freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
	FILE* fileOut = NULL;
	FILE* fileErr = NULL;
	FILE* fileIn = NULL;
	freopen_s(&fileOut, "CONOUT$", "w", stdout);
	freopen_s(&fileErr, "CONOUT$", "w", stderr);
	freopen_s(&fileIn, "CONIN$", "r", stdin);
}

void EngineExec(const wchar_t* command) {
	if (global_foutputdevice == 0) {
		reinterpret_cast<void* (*)(size_t, const wchar_t*, size_t)>(base_address + 0x01fca00)((__int64)((*GObjects)[0]) + 0x25ebde8, command, 0);
	}
	else {
		reinterpret_cast<void* (*)(size_t, const wchar_t*, size_t)>(base_address + 0x01fca00)((__int64)((*GObjects)[0]) + 0x25ebde8, command, global_foutputdevice);
	}
}

void tcpLinkOpen() {
	APoplarTcpLink* tcpLink = (APoplarTcpLink*)NewObject(APoplarTcpLink::StaticClass(), getLastOfType<UWorld>());
	tcpLink->WorldInfo = UObject::FindObject<AWorldInfo>("WorldInfo Wishbone_P.TheWorld.PersistentLevel.WorldInfo");
	tcpLink->LinkMode = ELinkMode::MODE_Binary;
	tcpLink->ReceiveMode = EReceiveMode::RMODE_Event;

	tcpLink->BindPort(0, false);

	FIpAddr parsedAddr = FIpAddr();

	TArray<uint8_t> addr = TArray<uint8_t>();

	addr.push_back(1);
	addr.push_back(0);
	addr.push_back(0);
	addr.push_back(127);
	parsedAddr.Addr = addr;
	parsedAddr.Port = 1337;

	tcpLink->Open(parsedAddr);
}

void tcpLinkListen() {
	APoplarTcpLink* tcpLink = (APoplarTcpLink*)NewObject(APoplarTcpLink::StaticClass(), getLastOfType<UWorld>());
	tcpLink->LinkMode = ELinkMode::MODE_Binary;
	tcpLink->ReceiveMode = EReceiveMode::RMODE_Event;
	
	tcpLink->AcceptClass = UTcpipConnection::StaticClass();

	std::cout << tcpLink->AcceptClass->GetFullName() << std::endl;

	tcpLink->WorldInfo = UObject::FindObject<AWorldInfo>("WorldInfo Wishbone_P.TheWorld.PersistentLevel.WorldInfo");

	tcpLink->BindPort(1337, true);

	tcpLink->Listen();
}

//ImportantListenFunc = 0x136e520
//undefined ImportantLookingFunc(longlong param_1,int param_2,longlong *param_3)

void UE3Listen() {
	std::cout << std::addressof(((UWorld*)nullptr)->NetDriver) << std::endl;

	getLastOfType<UGameEngine>()->CreateNamedNetDriver(FName(020724));

	UTcpNetDriver* NetDriver = getLastOfType<UTcpNetDriver>();

	UWorld* theWorld = UObject::FindObject<UWorld>("World Wishbone_P.TheWorld");

	theWorld->NetDriver = NetDriver;

	UObject::FindObject<AWorldInfo>("WorldInfo Wishbone_P.TheWorld.PersistentLevel.WorldInfo")->NetMode = ENetMode::NM_ListenServer;

	FURL furl = FURL();

	furl.Map = FString((const char*)L"Wishbone_P");
	furl.Port = 1337;
	furl.Valid = true;

	FString error = FString((const char*)L"");

	reinterpret_cast<void* (*)(UNetDriver* NetDriver, size_t world, FURL& url, FString& error)>(base_address + 0x0c21e30)(NetDriver, (__int64)theWorld + 0x58, furl, error);

	theWorld->NetDriver = NetDriver;

	NetDriver->NetConnectionClass = UTcpipConnection::StaticClass();

	std::cout << NetDriver->NetConnectionClass->GetFullName() << std::endl;

	//0x001ece0
	
	reinterpret_cast<void (*)(UPackageMap*)>(base_address + 0x001eb40)(getLastOfType<UPackageMap>());

	reinterpret_cast<void (*)(UPackageMap*)>(base_address + 0x001e8d0)(getLastOfType<UPackageMap>());
	
	//001e080

	//001e8d0

	//listening = true;

	//std::cout << NetDriver->NetConnectionClassName.ToString() << std::endl;

	/*
	 type InitListenFunc = fn(tcp_net_driver: usize, network_notify: usize, url: usize, string: usize) -> u64;

    let init_listen_func: InitListenFunc = std::mem::transmute(module_base_global + INIT_LISTEN_OFFSET);
	0x0c21e30*/
}

//longlong * FUN_1403ee460(longlong param_1,longlong *param_2)
AActor* SpawnActor(UWorld* world, UClass* actorClass) {
	return reinterpret_cast<AActor* (*)(UWorld* world, UClass* actorClass)>(base_address + 0x03ee460)(world, actorClass);
}

void setFOV(float newFOV) {
	getLastOfType<APoplarPlayerController>()->FOV(newFOV);
}

void describeAbility(UPoplarSkillDefinition* ability) {
	std::cout << ability->GetFullName() << std::endl;

	std::cout << ability->SkillEffectDefinitions.size() << std::endl;

	std::cout << ability->SkillEffectGroups.size() << std::endl;

	std::cout << ability->SkillEffectPresentations.size() << std::endl;

	std::cout << ability->SkillActivationActions.size() << std::endl;

	std::cout << ability->SkillDeactivationActions.size() << std::endl;

	if(ability->TrackedActionDefinition)
		std::cout << ability->TrackedActionDefinition->GetFullName() << std::endl;

	if(ability->ActionSkillArchetype)
		std::cout << ability->ActionSkillArchetype->GetFullName() << std::endl;

	//FSkillEffectData
}

void WebsocketListen() {
	WsServer server;

	server.config.port = 3884;

	auto& mainEndpoint = server.endpoint["/server"];

	mainEndpoint.on_open = [](std::shared_ptr<WsServer::Connection> connection) {
		std::cout << "Server: Opened connection " << connection.get() << std::endl;
		wsconnectionAckStatus.push_back(true);
		wsconnections.push_back(connection);
	};

	mainEndpoint.on_message = [](auto connection, auto message) {
		std::cout << "recieved ack!" << std::endl;
		for (int i = 0; i < wsconnections.size(); i++) {
			if (wsconnections[i].get() == connection.get()) {
				wsconnectionAckStatus[i] = true;
				std::cout << "ack'd connection " << i << std::endl;
			}
		}
	};

	std::promise<unsigned short> server_port;
	std::thread server_thread([&server, &server_port]() {
		// Start server
		server.start([&server_port](unsigned short port) {
			server_port.set_value(port);
			});
		});

	std::cout << "Websocket listening on port " << server_port.get_future().get() << std::endl
		<< std::endl;

	server_thread.join();
}

void WebsocketOpen() {
	WsClient client("127.0.0.1:3884/server");
	client.on_message = [](std::shared_ptr<WsClient::Connection> connection, std::shared_ptr<WsClient::InMessage> in_message) {
		std::cout << "Message Recieved!" << std::endl;
		std::vector<UActorChannel*> actorChannels = getActorChannels();

		HederaDefinitions::ReplicationFrame inRepFrame = HederaDefinitions::ReplicationFrame();

		inRepFrame.ParseFromString(in_message->string());

		replicationFrameQueue.push_back(inRepFrame);

		isClientAndConnected = true;
	};
	client.on_open = [](std::shared_ptr<WsClient::Connection> connection) {
		std::cout << "Client: Opened connection" << std::endl;
		clientWsConnection = connection;
	};

	client.start();
}

void rebornCPPMainThread() {
	init();
	initHooks();

	std::cout << std::hex << getLastOfType<APoplarPlayerReplicationInfo>()->VfTableObject.Dummy - base_address  << std::endl; //

	//listAllObjectsOfType<ULinkerLoad>();

	//EngineExec(L"open Wishbone_P");

	/*
	getLastOfType<APoplarGameReplicationInfo>()->bHardcoreMode = true;
	
	for (UHardcoreStatDimensionValueDefinition* obj : getAllObjectsOfType<UHardcoreStatDimensionValueDefinition>()) {
		if (obj->GetFullName().find("Default") == std::string::npos) {
			obj->bHardcoreEnabled = true;
		}
	}
	*/
	//listAllObjectsOfType<UHardcoreStatDimensionValueDefinition>();

	/*
	UPoplarGameInfoReplicatedVariableDefinition* opsPointsVariable = UObject::FindObject<UPoplarGameInfoReplicatedVariableDefinition>("PoplarGameInfoReplicatedVariableDefinition GD_GameTypes.ReplicatedVariables.GameInfoVar_PVEOperationsPoints");
	
	UBehavior_PoplarSetGameInfoVariable* setGameInfoBehaviorNewObject = (UBehavior_PoplarSetGameInfoVariable*)NewObject(UBehavior_PoplarSetGameInfoVariable::StaticClass(), opsPointsVariable->Outer);

	setGameInfoBehaviorNewObject->KeyDefinition = opsPointsVariable;
	setGameInfoBehaviorNewObject->NewValue = 100;

	TArray<UBehaviorBase*> behaviorBase = TArray<UBehaviorBase*>();

	behaviorBase.push_back(setGameInfoBehaviorNewObject);

	UBehaviorBase::RunBehaviors(setGameInfoBehaviorNewObject->Outer, nullptr, nullptr, FBehaviorParameters(), behaviorBase);
	*/

	//UBehavior_PoplarSetGameInfoVariable

	//std::cout << SPRI->ChallengeManager->CurrentDifficultyDimensionDef->GameDifficulty << std::endl;

	//SPRI->ChallengeManager->CurrentDifficultyDimensionDef->GameDifficulty = 50.0;

	//listAllObjectsOfType<UGBXDefinition>();
	
	while (true) {
		std::cout << "proc" << std::endl;

		std::string command = "";

		std::cin >> command;

		if (command.find("listen") != std::string::npos) {
			EngineExec(L"open Wishbone_P");

			Sleep(5 * 1000);

			getLastOfType<UEngine>()->bPauseOnLossOfFocus = false;

			UE3Listen();

			WebsocketListen();

			//tcpLinkListen();
		}
		else if (command.find("restartplayer") != std::string::npos) {
			APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();
			pc->ServerRestartPlayer();
		}
		else if (command.find("dumppcstate") != std::string::npos) {
			APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();

			std::cout << (int)pc->RemoteRole << std::endl;
			std::cout << (int)pc->Role << std::endl;
			std::cout << pc->Pawn << std::endl;
			std::cout << pc->PoplarPSI << std::endl;
			std::cout << pc->MyPoplarPRI << std::endl;
			if (pc->MyPoplarPRI) {
				std::cout << pc->MyPoplarPRI->Augs.AllCategories[0].Augs[0].AugDef << std::endl;
			}
			//tcpLinkListen();
		}
		else if (command.find("tm") != std::string::npos) {
			EngineExec(L"open Toby_Raid_P?bTournamentMode=1?SpawnBotsTeamA=2"); //SpawnBotsTeamA=4?SpawnBotsTeamB=5
			Sleep(5 * 1000);
			setFOV(110);

			//UBehavior_SpawnBot



			/*
			for (APoplarPlayerStateInfo* playerStateInfo : getAllObjectsOfType< APoplarPlayerStateInfo>()) {
				if (playerStateInfo->GetFullName().find("Default") == std::string::npos) {
					playerStateInfo
				}
			}
			*/

			//tcpLinkListen();
		}
		//else if (command.find("dumppcstate") != std::string::npos) {
			//APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();
		//}
		else if (command.find("gfx") != std::string::npos) {
			//1226120

			UObject* tst = reinterpret_cast<UObject* (*)()>(base_address + 0x1226120)();

			std::cout << tst->GetFullName() << std::endl;

			std::cout << std::hex <<  base_address << std::endl;

			std::cout << std::hex << tst->VfTableObject.Dummy << std::endl;

			getLastOfType<UPoplarPressStartGFxMovie>()->ContinueToMenu();
		}
		else if (command.find("fortnitefunny") != std::string::npos) {
			for (APoplarPlayerController* pc : getAllObjectsOfType<APoplarPlayerController>()) {
				if (pc->GetFullName().find("Default") == std::string::npos && pc->PlayerReplicationInfo->PlayerName.ToString().find("system") == std::string::npos) {
					std::cout << pc << std::endl;
					std::cout << pc->GetFullName() << std::endl;
					std::cout << pc->PlayerReplicationInfo->PlayerName.ToString() << std::endl;
					std::cout << pc->PendingSwapConnection << std::endl;
					std::cout << pc->CanRestartPlayer() << std::endl;
					std::cout << getLastOfType<APoplarGameInfo>()->PlayerCanRestart(pc) << std::endl;
					//pc->ServerRestartPlayer();

					APoplarGameInfo* gi = getLastOfType<APoplarGameInfo>();

					FString empty = FString();

					AActor* playerStart = gi->FindPlayerStart(pc, 1, empty);
					UClass* defaultClass = gi->GetDefaultPlayerClass(pc);

					pc->PlayerReplicationInfo->bHasBeenWelcomed = true;

					pc->Pawn = (APawn*)gi->Spawn(defaultClass, nullptr, FName(), playerStart->Location, playerStart->Rotation, nullptr, true);
					pc->Pawn->Controller = pc;
					pc->Pawn->PossessedBy(pc, false);
					pc->SetRotation(pc->Pawn->Rotation);
					//pc->GivePawn(pc->Pawn);

					pc->PlayerReplicationInfo->bOnlySpectator = false;
					pc->PoplarPSI->PoplarPawn = (APoplarPawn*)pc->Pawn;
					pc->PoplarPSI->Pawn = (APoplarPawn*)pc->Pawn;

					pc->Pawn->eventPreBeginPlay();
					pc->Pawn->eventPostBeginPlay();

					gi->eventRestartPlayer(pc);
				}
			}
		}
		else if (command.find("petoggle") != std::string::npos) {
			bStartPELogging = !bStartPELogging;
		}
		else if (command.find("dirty") != std::string::npos) {
			std::cout << "gri" << std::endl;
			for (APoplarPlayerReplicationInfo* pri : getAllObjectsOfType< APoplarPlayerReplicationInfo>()) {
				std::cout << pri->GetFullName() << std::endl;
				std::cout << pri->bOnlySpectator << std::endl;
				std::cout << pri->PlayerName.ToString() << std::endl;
				std::cout << pri->StartTime << std::endl;
			}
		}
		else if (command.find("vpnopen") != std::string::npos) {
			EngineExec(L"open 26.15.25.193:1337");

			//FString addr = FString((const char*)L"127.0.0.1:1337");

			//getLastOfType<APoplarGameInfoFrontend>()->DebugForceConnectToCustomServer(addr);

			//EngineExec(L"open Wishbone_P");

			//Sleep(5 * 1000);

			//EngineExec(L"open 127.0.0.1:1337");
			//tcpLinkOpen();
			}
		else if (command.find("open") != std::string::npos) {
			EngineExec(L"open 127.0.0.1:1337");
			WebsocketOpen();

			//FString addr = FString((const char*)L"127.0.0.1:1337");

			//getLastOfType<APoplarGameInfoFrontend>()->DebugForceConnectToCustomServer(addr);

			//EngineExec(L"open Wishbone_P");

			//Sleep(5 * 1000);

			//EngineExec(L"open 127.0.0.1:1337");
			//tcpLinkOpen();
		}
		else if (command.find("bots") != std::string::npos) {
			EngineExec(L"open Wishbone_P");

			Sleep(4 * 1000);

			//UBehavior_SpawnBot

			//PoplarMetaSkinDefinition GD_RocketHawk_DAH.Skins.SkinId_Color017

			/*
			APoplarGameInfo* gi = getLastOfType<APoplarGameInfo>();

			UBehavior_SpawnBot* spawnBotBehavior = (UBehavior_SpawnBot*)NewObject(UBehavior_SpawnBot::StaticClass(), gi);

			spawnBotBehavior->Characters = TArray< UPoplarPlayerNameIdentifierDefinition*>();
			spawnBotBehavior->Characters.push_back(UObject::FindObject<UPoplarPlayerNameIdentifierDefinition>("PoplarPlayerNameIdentifierDefinition GD_RocketHawk.NameId_RocketHawk"));
			spawnBotBehavior->CharacterIndex = 0;

			spawnBotBehavior->Teams = TArray<UPoplarTeamDefinition*>();
			spawnBotBehavior->Teams.push_back(getAllObjectsOfType<UPoplarTeamDefinition>()[2]);
			spawnBotBehavior->TeamIndex = 0;

			spawnBotBehavior->Skins = TArray< UPoplarMetaSkinDefinition*>();
			spawnBotBehavior->Skins.push_back(UObject::FindObject<UPoplarMetaSkinDefinition>("PoplarMetaSkinDefinition GD_RocketHawk_DAH.Skins.SkinId_Color017"));
			spawnBotBehavior->SkinIndex = 0;

			spawnBotBehavior->Difficulty = 1;

			TArray<UBehaviorBase*> behaviorBase = TArray<UBehaviorBase*>();

			behaviorBase.push_back(spawnBotBehavior);

			UBehaviorBase::RunBehaviors(gi, nullptr, nullptr, FBehaviorParameters(), behaviorBase);
			*/
		}
		else if (command.find("possessremote") != std::string::npos) {
			for (APoplarPlayerController* pc : getAllObjectsOfType<APoplarPlayerController>()) {

				if (!pc->IsLocalPlayerController() && pc->GetFullName().find("Default") == std::string::npos) {
					pc->PlayerReplicationInfo->bOnlySpectator = true;
					pc->ClientReset();
				}
			}
		}
		else if (command.find("possess") != std::string::npos) {
			for (APoplarPlayerController* pc : getAllObjectsOfType<APoplarPlayerController>()) {

				if (pc->IsLocalPlayerController()) {
					pc->Possess(getLastOfType<APoplarPlayerPawn>(), false);
					pc->GivePawn(getLastOfType<APoplarPlayerPawn>());
					pc->eventBeginState(FName(1830));

					std::cout << pc->GetStateName().ToString() << std::endl;
				}
			}
		}
		else if (command.find("listacs") != std::string::npos) {
			for (UActorChannel* ac : getAllObjectsOfType<UActorChannel>()) {
				if(ac->Actor)
					std::cout << ac->Actor->GetFullName() << std::endl;
			}
		}
		else if (command.find("champselect") != std::string::npos) {
			//listAllObjectsOfType< UPoplarMatchStateExpressionEvaluator>();
			//OpenCharacterSelectLevel
			//getLastOfType< APoplarPlayerController>()->CurrentCharacterSelectMovieState
			//getLastOfType< APoplarPlayerController>()->OpenCharacterSelectLevel();
			//getLastOfType< APoplarPlayerController>()->OpenCharacterSelectMovie();
		}
		else if (command.find("idle") != std::string::npos) {
			//ClientNotifyIdle
			getLastOfType<APoplarPlayerController>()->ClientNotifyIdle();
		}
		else if (command.find("list") != std::string::npos) {
			std::cout << "PCs" << std::endl;
			for (APoplarPlayerController* pc : getAllObjectsOfType<APoplarPlayerController>()) {
				std::cout << pc->GetFullName() << std::endl;
				std::cout << (int)pc->Role << std::endl;
				std::cout << (int)pc->RemoteRole << std::endl;

				if (pc->IsLocalPlayerController()) {
					std::cout << "local" << std::endl;
					for (APoplarPlayerReplicationInfo* pri : getAllObjectsOfType<APoplarPlayerReplicationInfo>()) {
						if (pri->PlayerName.ToString() == "" && pri->GetFullName().find("Default") == std::string::npos) {
							std::cout << "assigned" << std::endl;
							pc->PlayerReplicationInfo = pri;
							pc->MyPoplarPRI = pri;
						}
					}
				}
				else {
					//if (pc->GetFullName().find("Default"));
				}
			}
			std::cout << "PRIs" << std::endl;
			for (APoplarPlayerReplicationInfo* pri : getAllObjectsOfType<APoplarPlayerReplicationInfo>()) {
				std::cout << pri->GetFullName() << std::endl;
				std::cout << pri->PlayerName.ToString() << std::endl;
				//std::cout << pc->MyPoplarPRI << std::endl;
			}
		}
		else if (command.find("lp") != std::string::npos) {
			UWillowLocalPlayer* lp = getLastOfType<UWillowLocalPlayer>();
			std::cout << lp->Actor << std::endl;
		}
		else if (command.find("diff1") != std::string::npos) {
			EngineExec(L"open Portal_P");

			Sleep(5 * 1000);

			setFOV(110);

			//PoplarGameReplicationInfo Toby_Raid_P.TheWorld.PersistentLevel.PoplarGameReplicationInfo

			for (UDifficultyStatDimensionValueDefinition* stat : getAllObjectsOfType< UDifficultyStatDimensionValueDefinition>()) {
				if (stat->GetFullName().find("Default") == std::string::npos) {
					std::cout << stat->GetFullName() << ": " << stat->GameDifficulty << std::endl;
					//stat->GameDifficulty = 2;
				}
			}
		}
		else if (command.find("diff2") != std::string::npos) {
			EngineExec(L"open Portal_P");

			Sleep(5 * 1000);

			setFOV(110);

			//PoplarGameReplicationInfo Toby_Raid_P.TheWorld.PersistentLevel.PoplarGameReplicationInfo

			for (UDifficultyStatDimensionValueDefinition* stat : getAllObjectsOfType< UDifficultyStatDimensionValueDefinition>()) {
				if (stat->GetFullName().find("Default") == std::string::npos) {
					std::cout << stat->GetFullName() << ": " << stat->GameDifficulty << std::endl;
					stat->GameDifficulty = 99.0;
				}
			}
		}
		else if (command.find("timer") != std::string::npos) {
			getLastOfType<APoplarGameReplicationInfo>()->RemainingTime.bCounting = false;
			getLastOfType<APoplarGameReplicationInfo>()->RemainingTime.bCountUp = false;
			getLastOfType<APoplarGameReplicationInfo>()->RemainingTime.bRealtime = false;
			getLastOfType<APoplarGameReplicationInfo>()->RemainingTime.Time = 696969696.69f;
			getLastOfType<APoplarGameReplicationInfo>()->RemainingTime.BaseTimeValue = 696969696.69f;
		}
		else if (command.find("test") != std::string::npos) {
			//UPoplarConsole
			//reinterpret_cast<void (*)(UEngine*, const wchar_t*, void*)>(base_address + 0x05dbd90)(getLastOfType<UEngine>(), L"DumpConsoleCommands", nullptr);
			//EngineExec(L"open Wishbone_P?listen");

			//3495afa

			//reinterpret_cast<void (*)()>(base_address + 0x05e4450)();

			//0x05e4450

			//EngineExec(L"open Wishbone_P");

			//Sleep(5 * 1000);

			//2812F80

			for (UPoplarMetaItemPackDefinition* pack : getAllObjectsOfType< UPoplarMetaItemPackDefinition>()) {
				std::cout << "- " << pack->GetFullName() << std::endl;

				for (UPoplarMetaItemProbabilityDefinition* probabilityDefinition : pack->PackItemSlots) {
					if (probabilityDefinition) {
						if (probabilityDefinition->IsA< UPoplarPerkProbabilityDefinition>()) {
							UPoplarPerkProbabilityDefinition* newProbabilityDef = (UPoplarPerkProbabilityDefinition*)probabilityDefinition;

							std::cout << "		- " << newProbabilityDef->GetFullName() << std::endl;

							for (FItemProbability probability : newProbabilityDef->ItemProbabilities) {
								if (probability.ItemToCreate)
									std::cout << "				- " << probability.ItemToCreate->ItemName.ToString() << ": " << probability.Probability << std::endl;
							}
						}
						else {
							std::cout << "		- " << probabilityDefinition->GetFullName() << std::endl;

							for (FItemProbability probability : probabilityDefinition->ItemProbabilities) {
								if (probability.ItemToCreate)
									std::cout << "				- " << probability.ItemToCreate->ItemName.ToString() << ": " << probability.Probability << std::endl;
							}
						}
					}
				}
			}

			/*
			UPoplarPerkFunction* pf = getLastOfType< UPoplarPerkFunction>();

			std::cout << pf->GetFullName() << std::endl;
			std::cout << pf->HydraApolloData.ToString() << std::endl;
			std::cout << pf->HydraItemWebToolAddress.ToString() << std::endl;
			std::cout << pf->
			*/

			//(*(char*)(base_address + 0x3495afa)) = '\x01';
		}
		else if (command.find("gra") != std::string::npos) {
			//UPoplarConsole
			//reinterpret_cast<void (*)(UEngine*, const wchar_t*, void*)>(base_address + 0x05dbd90)(getLastOfType<UEngine>(), L"DumpConsoleCommands", nullptr);
			//EngineExec(L"open Wishbone_P?listen");

			//34e04a0

			EngineExec(L"open Wishbone_P?listen");

			Sleep(5 * 1000);
		}
		//ULevelListStatDimensionValueDefinition
		else if (command.find("chal") != std::string::npos) {
			getLastOfType<APoplarPlayerController>()->Behavior_IncrementStat((UStatDefinition*)UObject::FindObject<UStatDefinition>("StatDefinition GD_TobyRaid_Challenges.Stats.LevelMatchesPlayedTobyRaid"), 2);

			EngineExec(L"open Toby_Raid_P");

			Sleep(5 * 1000);

			setFOV(110);

			//StatDefinition GD_TobyRaid_Challenges.Stats.LevelMatchesPlayedTobyRaid
		}
		else if (command.find("bf") != std::string::npos) {
			EngineExec(L"open Toby_Raid_P");

			Sleep(5 * 1000);

			listAllObjectsOfType< UDifficultyStatDimensionValueDefinition>();

			for (UDifficultyStatDimensionValueDefinition* difficulty : getAllObjectsOfType< UDifficultyStatDimensionValueDefinition>()) {
				std::cout << difficulty->GetFullName() << std::endl;
				std::cout << difficulty->GameDifficulty << std::endl;

				if (difficulty->GetFullName().find("Default") == std::string::npos) {
					difficulty->GameDifficulty = 99;
				}
			}
		}
		else if (command.find("currencycheck") != std::string::npos) {
			UPoplarGameInfoReplicatedVariableDefinition* scoreVar = UObject::FindObject< UPoplarGameInfoReplicatedVariableDefinition>("PoplarGameInfoReplicatedVariableDefinition GD_GameTypes.ReplicatedVariables.GameInfoVar_PVEScore");
			
			UBehavior_PoplarGetGameInfoVariable* setGameInfoBehaviorNewObject = (UBehavior_PoplarGetGameInfoVariable*)NewObject(UBehavior_PoplarGetGameInfoVariable::StaticClass(), scoreVar->Outer);

			setGameInfoBehaviorNewObject->KeyDefinition = scoreVar;

			TArray<UBehaviorBase*> behaviorBase = TArray<UBehaviorBase*>();

			behaviorBase.push_back(setGameInfoBehaviorNewObject);

			UBehaviorBase::RunBehaviors(setGameInfoBehaviorNewObject->Outer, nullptr, nullptr, FBehaviorParameters(), behaviorBase);
		}
		//APoplarMetaPlayerReplicationInfo
		else if (command.find("spec") != std::string::npos) {
			for (APoplarPlayerController* pc : getAllObjectsOfType<APoplarPlayerController>()) {
				if (pc->GetFullName().find("Default") == std::string::npos && (__int64)pc->MyPoplarPRI != 0) {
					std::cout << pc->MyPoplarPRI->PlayerName.ToString() << std::endl;
					std::cout << (int)pc->MyPoplarPRI->CurrentPlayerState << std::endl;

					if (pc->MyPoplarPRI->CurrentPlayerState == EPlayerState::PlayerState_WaitingToStart) {
						//pc->MyPoplarPRI->CurrentPlayerState = ;
						pc->MyPoplarPRI->ServerAdvanceCurrentPlayerState(EPlayerState::PlayerState_Playing);
					}
					
				}
			}
		}
		else if (command.find("associate") != std::string::npos) {
			for (APoplarPlayerController* pc : getAllObjectsOfType<APoplarPlayerController>()) {
				if (pc->GetFullName().find("Default") == std::string::npos && (__int64)pc->MyPoplarPRI == 0) {
					for (APoplarPlayerReplicationInfo* pri : getAllObjectsOfType< APoplarPlayerReplicationInfo>()) {
						if (pri->GetFullName().find("Default") == std::string::npos && pri->PlayerName.empty() && pri->CurrentPlayerState == EPlayerState::PlayerState_WaitingToStart) {
							pc->MyPoplarPRI = pri;
							pc->PlayerReplicationInfo = pri;
							//pri->ServerAdvanceCurrentPlayerState(EPlayerState::PlayerState_Playing);
							pc->eventReceivedPlayer();
							pc->AfterReceivedPlayerWaitForCharacterLoad();
							std::cout << "ass complete" << std::endl;
						}
					}

				}
			}
		}
		else if (command.find("ability") != std::string::npos) {
			EngineExec(L"open Snowblind_P");

			Sleep(4 * 1000);

			getLastOfType<APoplarPlayerController>()->eventSwitchPoplarPlayerClass(UObject::FindObject<UPoplarPlayerNameIdentifierDefinition>("PoplarPlayerNameIdentifierDefinition GD_RocketHawk.NameId_RocketHawk"));

			Sleep(2 * 1000);

			listAllObjectsOfType< UPoplarActionSkillDefinition>();

			APoplarPlayerStateInfo* psi = getLastOfType< APoplarPlayerStateInfo>();

			describeAbility(UObject::FindObject<UPoplarSkillDefinition>("PoplarSkillDefinition GD_ModernSoldier_Skills.Cloak.Skill_Cloak"));//

			psi->ActionSkillSlotToSkillTable[0]->ActionSkillDefinition = UObject::FindObject<UPoplarActionSkillDefinition>("PoplarActionSkillDefinition GD_ModernSoldier_Skills.FragGrenade.A_FragGrenade");
			//psi->ActionSkillSlotToSkillTable[0]->trackedaction = UObject::FindObject<UPoplarActionSkillDefinition>("PoplarActionSkillDefinition GD_ModernSoldier_Skills.FragGrenade.A_FragGrenade");

			std::cout << "gra" << std::endl;

			//PoplarSkillDefinition GD_ModernSoldier_Skills.Airstrike.Skill_Airstrike

			//APoplarPlayerStateInfo
			//class UPoplarSkillDefinition*                      ActionSkillSlotToSkillTable[0x5]; 
		}
		else if (command.find("servertravel") != std::string::npos) {
			getLastOfType<APoplarPlayerController>()->MatchStarting();
		}
		else if (command.find("split1v1") != std::string::npos) {
			EngineExec(L"open Dojo_P");

			Sleep(15 * 1000);

			UGameViewportClient* vpc = getLastOfType< UGameViewportClient>();

			vpc->SetSplitscreenConfiguration(ESplitScreenType::eSST_2P_VERTICAL);
			vpc->UpdateActiveSplitscreenType();

			FPlatformUserId userID = FPlatformUserId();

			userID.bHasValue = true;
			userID.RawId[0] = 0x1;

			FString err = FString();

			vpc->CreatePlayer(userID, true, err);

			//bStartPELogging = false;

			getLastOfType<APoplarPlayerController>()->MyPoplarPRI->SetPlayerTeam(getAllObjectsOfType<ATeamInfo>()[1]);
		}
		else if (command.find("split") != std::string::npos) {
			listAllObjectsOfType<UGameViewportClient>();

			EngineExec(L"open Toby_Raid_P?bTournamentMode=1");

			Sleep(5 * 1000);

			UGameViewportClient* vpc = getLastOfType< UGameViewportClient>();

			//vpc->SetSplitscreenConfiguration(ESplitScreenType::eSST_2P_VERTICAL);
			//vpc->UpdateActiveSplitscreenType();

			FPlatformUserId userID = FPlatformUserId();

			userID.bHasValue = true;
			userID.RawId[0] = 0x1;

			FString err = FString();

			bStartPELogging = true;

			vpc->CreatePlayer(userID, true, err);

			//bStartPELogging = false;

			std::cout << "Createplayer error: " << err.ToString() << std::endl;
		}
		else if (command.find("gear") != std::string::npos) {
			//listAllObjectsOfType< UPoplarPerkFunction>();

			//EngineExec(L"open Wishbone_P");

			//Sleep(5 * 1000);

			APoplarPlayerReplicationInfo* PoplarPRI = getLastOfType<APoplarPlayerReplicationInfo>();

			FReplicatedPerkItem perk = FReplicatedPerkItem();

			UPoplarPerkFunction* perkFunction = UObject::FindObject<UPoplarPerkFunction>("PoplarPerkFunction GD_Gear_DAH.Gear.PF_Gear_AttackDamage_Legendary_ROG2");

			perkFunction->ItemLevelOverride = 2147483647;
			perkFunction->bUseItemLevelOverride = true;

			APoplarPlayerController* poplarPC = getLastOfType<APoplarPlayerController>();

			perk.bActive = false;
			perk.bCanUse = true;
			perk.PerkFunction = perkFunction;
			perk.MetaID = perkFunction->MetaContentID;
			perk.AssetKey = perkFunction->AssetTrackerKey;
			perk.Rarity = (int)EPerkRarity::PERKRARITY_Legendary;
			//std::cout << perk.ItemLevel << std::endl;

			FReplicatedPerkItem oldPerk = PoplarPRI->Perks[0];
			PoplarPRI->Perks[0] = perk;
			PoplarPRI->OnRep_Perks(0, oldPerk);
		}
		else if (command.find("toby") != std::string::npos) {
			//StatDefinition GD_TobyRaid_Challenges.Stats.LevelMatchesWonTobyRaid
			APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();

			UStatDefinition* tobyStat = UObject::FindObject<UStatDefinition>("StatDefinition GD_TobyRaid_Challenges.Stats.LevelOperationResetBucketOneTobyRaid");

			//pc->Behavior_UpdateStat(tobyStat, EStatCategoryTypes::STAT_CAwTEGORY_CAREER, )
			//UObject::FindObject<UStatDefinition>("StatDefinition GD_TobyRaid_Challenges.Stats.LevelMatchesWonTobyRaid");
			
			EngineExec(L"open Toby_Raid_P");
		}
		else if (command.find("owo") != std::string::npos) {
			EngineExec(L"open Wishbone_P");

			Sleep(5 * 1000);

			APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();

			pc->FOV(110);

			UGameViewportClient* viewportClient = getLastOfType< UGameViewportClient>();

			FPlatformUserId userID = FPlatformUserId();

			userID.bHasValue = true;
			userID.RawId[0] = 0x1;

			FString err = FString();

			viewportClient->CreatePlayer(userID, true, err);
		}
		else if (command.find("skin") != std::string::npos) {
			EngineExec(L"open Snowblind_P");

			Sleep(4 * 1000);

			//void eventServerSelectCharacterSkin(class UPoplarMetaSkinDefinition* SelectedSkin);
			//listAllObjectsOfType< UPoplarMetaSkinDefinition>();
			//PoplarMetaSkinDefinition GD_Modernsoldier_DAH.Skins.SkinId_Color021
			getLastOfType<APoplarPlayerController>()->eventServerSelectCharacterSkin(UObject::FindObject<UPoplarMetaSkinDefinition>("PoplarMetaSkinDefinition GD_Modernsoldier_DAH.Skins.SkinId_Color021"));
			getLastOfType<APoplarPlayerController>()->ServerRestartPlayer();
		}
		else if (command.find("consolea") != std::string::npos) {
			getLastOfType<UPoplarConsole>()->InputChar(FPlatformUserId(), FString((const char*)L"A"));
		}
		else if (command.find("console") != std::string::npos) {
			if (static_construct_object_param_9) {
				getLastOfType<UGameViewportClient>()->ViewportConsole = (UConsole*)StaticConstructObjectHook(UConsole::StaticClass(), (size_t)getLastOfType<UGameViewportClient>(), 0, 0, 0, 0, 0, 0, static_construct_object_param_9);
				getLastOfType<UEngine>()->ConsoleClass = UConsole::StaticClass();
				getLastOfType<UEngine>()->ConsoleClassName = FString((const char*)L"UConsole");
				getLastOfType<UGameViewportClient>()->SetConsoleTarget(0);
				//getLastOfType<APlayerController>()->ConsoleKey(FName(31782));
				getLastOfType<UConsole>()->Initialized();
				getLastOfType<UConsole>()->StartTyping(FString());
				//getLastOfType<UPoplarConsole>()->StartTyping(FString((const char*)L"open Toby_Raid_P"));
				//getLastOfType<UPoplarConsole>()->bCaptureKeyInput = true;
			}
			//getLastOfType<APlayerController>()->ConsoleKey
			else {
				std::cout << "Param 9 missing!" << std::endl;
			}
		}
		else if (command.find("mutation") != std::string::npos) {
			APoplarPlayerReplicationInfo* PoplarPRI = getLastOfType<APoplarPlayerReplicationInfo>();

			listAllObjectsOfType< UPoplarAugDefinition>();

			PoplarPRI->Augs.AllCategories[0].Mutation.AugDef = UObject::FindObject<UPoplarAugDefinition>("PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_PassiveCrits");
			PoplarPRI->Augs.AllCategories[0].CategoryIsUnlocked = true;
			PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef = UObject::FindObject<UPoplarAugDefinition>("PoplarAugDefinition GD_ModernSoldier_Skills.PassiveAugs.Aug_ModernSoldier_PassiveMoreRounds");
			//PoplarAugDefinition GD_ModernSoldier_Skills.PassiveAugs.Aug_ModernSoldier_PassiveMoreRounds

			PoplarPRI->Augs.AllCategories[4].Mutation.AugDef = UObject::FindObject<UPoplarAugDefinition>("PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_PassiveCrits");
			PoplarPRI->Augs.AllCategories[4].CategoryIsUnlocked = true;
		}
		else if (command.find("scopes") != std::string::npos) {
			APoplarPlayerReplicationInfo* PoplarPRI = getLastOfType<APoplarPlayerReplicationInfo>();

			listAllObjectsOfType< UPoplarAugDefinition>();

			PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef = PoplarPRI->Augs.AllCategories[2].Augs[0].AugDef;
			PoplarPRI->Augs.AllCategories[1].Augs[0].AugDef = PoplarPRI->Augs.AllCategories[2].Augs[1].AugDef;
			//
			//PoplarAugDefinition GD_ModernSoldier_Skills.PassiveAugs.Aug_ModernSoldier_PassiveMoreRounds
		}
		else if (command.find("pause") != std::string::npos) {
			getLastOfType<UEngine>()->bPauseOnLossOfFocus = false;
		}
		else if (command.find("cmut") != std::string::npos) {
			APoplarPlayerReplicationInfo* PoplarPRI = getLastOfType<APoplarPlayerReplicationInfo>();

			listAllObjectsOfType< UPoplarAugDefinition>();

			UPoplarAugDefinition* newAug = (UPoplarAugDefinition*)NewObject(UPoplarAugDefinition::StaticClass(), PoplarPRI);

			newAug->Icon = PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef->Icon;
			newAug->IconMovie = PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef->IconMovie;
			newAug->IconPath = PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef->IconPath;

			std::cout << PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef->AugName.ToString() << std::endl;

			newAug->AugName = FString((const char*)L":3");
			newAug->Description = FString((const char*)L"OwO");
			newAug->FullyPurchasedDescription = FString((const char*)L"OwO");
			newAug->SkillSlotIndicator = PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef->SkillSlotIndicator;

			PoplarPRI->Augs.AllCategories[0].Mutation.AugDef = newAug;

			//PoplarPRI->Augs.AllCategories[0].Mutation.AugDef = UObject::FindObject<UPoplarAugDefinition>("PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_PassiveCrits");
			//PoplarPRI->Augs.AllCategories[0].CategoryIsUnlocked = true;
			//PoplarPRI->Augs.AllCategories[0].Augs[0].AugDef = UObject::FindObject<UPoplarAugDefinition>("PoplarAugDefinition GD_ModernSoldier_Skills.PassiveAugs.Aug_ModernSoldier_PassiveMoreRounds");
			//PoplarAugDefinition GD_ModernSoldier_Skills.PassiveAugs.Aug_ModernSoldier_PassiveMoreRounds

			//PoplarPRI->Augs.AllCategories[4].Mutation.AugDef = UObject::FindObject<UPoplarAugDefinition>("PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_PassiveCrits");
			//PoplarPRI->Augs.AllCategories[4].CategoryIsUnlocked = true;
		}
		else if (command.find("proc") != std::string::npos) {
			EngineExec(L"open Wishbone_P");

			Sleep(8 * 1000);

			APoplarGameInfo* gameInfo = getLastOfType<APoplarGameInfo>();

			((AWorldInfo*)UObject::FindObject<AWorldInfo>("WorldInfo Wishbone_P.TheWorld.PersistentLevel.WorldInfo"))->NetMode = ENetMode::NM_ListenServer;

			FString options = FString((const char*)L"");
			FString address = FString((const char*)L"127.0.0.1");
			FString outError = FString((const char*)L"");

			FUniqueNetId netID = FUniqueNetId();

			netID.bHasValue = true;
			netID.RawId[0] = 0x1;

			gameInfo->eventPreLogin(options, address, netID, outError);

			FString portal = FString((const char*)L"");
			FString params = FString((const char*)L"");
			FString error = FString((const char*)L"");

			APoplarPlayerController* pc = (APoplarPlayerController*)gameInfo->eventLogin(portal, params, netID, error);

			std::cout << "prelogin error: " << outError.ToString() << std::endl;
			std::cout << "login error: " << error.ToString() << std::endl;

			AActor* playerStart = gameInfo->FindPlayerStart(pc, 1, portal);
			UClass* defaultClass = gameInfo->GetDefaultPlayerClass(pc);

			std::cout << playerStart << std::endl;
			std::cout << defaultClass << std::endl;

			pc->PlayerReplicationInfo->bHasBeenWelcomed = true;

			pc->Pawn = (APawn*)gameInfo->Spawn(defaultClass, nullptr, FName(), playerStart->Location, playerStart->Rotation, nullptr, true);
			pc->Pawn->Controller = pc;
			pc->Pawn->PossessedBy(pc, false);
			pc->SetRotation(pc->Pawn->Rotation);
			//pc->GivePawn(pc->Pawn);

			pc->PlayerReplicationInfo->bOnlySpectator = false;
			pc->PoplarPSI->PoplarPawn = (APoplarPawn*)pc->Pawn;
			pc->PoplarPSI->Pawn = (APoplarPawn*)pc->Pawn;

			pc->Pawn->eventPreBeginPlay();
			pc->Pawn->eventPostBeginPlay();

			//gameInfo->eventPostLogin(pc);

			//listAllObjectsOfType<ATeamInfo>();

			//((AGameInfo*)gameInfo)->eventRestartPlayer(pc);

			//APoplarPawn* pawn = (APoplarPawn*)SpawnActor(world, APoplarPawn::StaticClass());

			//


			//Sleep(5 * 1000);

			std::cout << (__int64)pc->PoplarPSI << std::endl;
			std::cout << (__int64)pc->PlayerReplicationInfo << std::endl;
			std::cout << (__int64)pc->Pawn << std::endl;

			FString name = FString((const char*)L"gwog");

			pc->ServerSetPlayerName(name);

			pc->eventSwitchPoplarPlayerClass(UObject::FindObject<UPoplarPlayerNameIdentifierDefinition>("PoplarPlayerNameIdentifierDefinition GD_RocketHawk.NameId_RocketHawk"));
		}

		std::cout << "endproc" << std::endl;
	}

	/*
	std::cout << "you think you the shit bitch" << std::endl;

	APoplarPlayerReplicationInfo* PoplarPRI = getLastOfType<APoplarPlayerReplicationInfo>();

	FReplicatedPerkItem perk = FReplicatedPerkItem();

	UPoplarPerkFunction* perkFunction = UObject::FindObject<UPoplarPerkFunction>("PoplarPerkFunction GD_Gear_DAH.Gear.PF_Gear_AttackDamage_Legendary_ROG2");

	perk.bActive = false;
	perk.bCanUse = true;
	perk.PerkFunction = perkFunction;
	perk.MetaID = perkFunction->MetaContentID;
	perk.AssetKey = perkFunction->AssetTrackerKey;

	FReplicatedPerkItem oldPerk = PoplarPRI->Perks[0];
	PoplarPRI->Perks[0] = perk;
	PoplarPRI->OnRep_Perks(0, oldPerk);

	std::cout << "you not even the fart (gra)" << std::endl;

	PoplarPRI->Augs.AllCategories[4].Mutation.AugDef = UObject::FindObject<UPoplarAugDefinition>("PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_PassiveCrits");
	PoplarPRI->Augs.AllCategories[4].CategoryIsUnlocked = true;

	std::cout << "i be going hard (ya)" << std::endl;

	APoplarPlayerController* pc = getLastOfType<APoplarPlayerController>();
	*/

	//std::cout << "bitches be rich but im richer" << std::endl;

	//getLastOfType<AWorldInfo>()->NetMode = ENetMode::NM_Standalone;
	//getLastOfType<APoplarPlayerController>()->EnableCheats();

	//std::cout << (__int64)getLastOfType<APoplarPlayerController>()->CheatManagerObject << std::endl;

	/*
	PoplarAugDefinition GD_ModernSoldier_Skills.AirstrikeAugs.Aug_ModernSoldier_AirstrikeLaser
PoplarAugDefinition GD_ModernSoldier_Skills.AirstrikeAugs.Aug_ModernSoldier_AirstrikeSlow
PoplarAugDefinition GD_ModernSoldier_Skills.CloakAugs.Aug_ModernSoldier_CloakDuration
PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_CloakGhostAirstrike
PoplarAugDefinition GD_ModernSoldier_Skills.CloakAugs.Aug_ModernSoldier_CloakShieldChargeRate
PoplarAugDefinition GD_ModernSoldier_Skills.CloakAugs.Aug_ModernSoldier_CloakSpeedBoost
PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_FragConcentrated
PoplarAugDefinition GD_ModernSoldier_Skills.FragGrenadeAugs.Aug_ModernSoldier_FragCooldown
PoplarAugDefinition GD_ModernSoldier_Skills.FragGrenadeAugs.Aug_ModernSoldier_FragDamage
PoplarAugDefinition GD_ModernSoldier_Skills.FragGrenadeAugs.Aug_ModernSoldier_FragImpact
PoplarAugDefinition GD_ModernSoldier_Skills.FragGrenadeAugs.Aug_ModernSoldier_FragMIRV
PoplarAugDefinition GD_ModernSoldier_Skills.FragGrenadeAugs.Aug_ModernSoldier_FragRadius
PoplarAugDefinition GD_ModernSoldier_Skills.FragGrenadeAugs.Aug_ModernSoldier_FragRange
PoplarAugDefinition GD_ModernSoldier_Skills.PassiveAugs.Aug_ModernSoldier_PassiveMoreRounds
PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_PassiveCrits
PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_WeaponDamage
PoplarAugDefinition GD_ModernSoldier_Skills.CharacterAugs.Aug_ModernSoldier_CharacterSpeed
PoplarAugDefinition GD_ModernSoldier_Skills.CloakAugs.Aug_ModernSoldier_CloakCooldownReduction
PoplarAugDefinition GD_ModernSoldier_Skills.CloakAugs.Aug_ModernSoldier_CloakDamageBurst
PoplarAugDefinition GD_ModernSoldier_Skills.FragGrenadeAugs.Aug_ModernSoldier_FragNapalm
PoplarAugDefinition GD_ModernSoldier_Skills.PassiveAugs.Aug_ModernSoldier_PassiveShieldPen
PoplarAugDefinition GD_ModernSoldier_Skills.WeaponAugs.Aug_ModernSoldier_WeaponShieldPen
PoplarAugDefinition GD_ModernSoldier_Skills.Augmentations.Aug_ModernSoldier_AirstrikeChasing
PoplarAugDefinition GD_ModernSoldier_Skills.WeaponAugs.Aug_ModernSoldier_WeaponSight
PoplarAugDefinition GD_ModernSoldier_Skills.WeaponAugs.Aug_ModernSoldier_WeaponScope
PoplarAugDefinition GD_ModernSoldier_Skills.CloakAugs.Aug_ModernSoldier_CloakShieldChargeReset*/

	//listAllObjectsOfType<UPoplarAugDefinition>();
	//listAllObjectsOfType<UPoplarAugCategory>();
	//UPoplarAugSet

	/*
	APoplarMetaPlayerReplicationInfo* metaPRI = getLastOfType<APoplarMetaPlayerReplicationInfo>();

	metaPRI->CurrentLoadout.Index = 0;
	metaPRI->CurrentLoadout.Version = 0;
	metaPRI->CurrentLoadout.LoadoutName = FString((const char*)L"gwog's loadout");

	FReplicatedPerkItem perk = FReplicatedPerkItem();

	UPoplarPerkFunction* perkFunction = UObject::FindObject<UPoplarPerkFunction>("PoplarPerkFunction GD_Gear_DAH.Gear.PF_Gear_AttackDamage_Legendary_ROG2");

	perk.bActive = false;
	perk.bCanUse = true;
	perk.PerkFunction = perkFunction;
	perk.MetaID = perkFunction->MetaContentID;
	perk.AssetKey = perkFunction->AssetTrackerKey;

	//listAllObjectsOfType<UPoplarPerkFunction>();
	//listAllObjectsOfType<UPoplarPlayerWearableDefinition>();

	metaPRI->CurrentLoadout.Perks[0] = perk;
	metaPRI->
	*/
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ulReasonForCall, LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule);

	if (ulReasonForCall != DLL_PROCESS_ATTACH)
		return TRUE;

	HANDLE hThread = CreateThread(nullptr, 0,
		(LPTHREAD_START_ROUTINE)rebornCPPMainThread, hModule, 0, 0);
	if (hThread != nullptr)
		CloseHandle(hThread);

	// TODO: Fill your code here

	return TRUE;
}

