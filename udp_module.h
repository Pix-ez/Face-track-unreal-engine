#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"

#include "udp_module.generated.h"
#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(1, 1.5f, FColor::White, text, false);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDataReceivedDelegate, const FString&, Data);

UCLASS()
class POSE_API Audp_module : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, Category = "Center")
		float CenterY;

	UPROPERTY(BlueprintReadOnly, Category = "Center")
		float CenterZ;


	Audp_module();
	~Audp_module();

	FSocket* Socket;

	// Local Endpoint
	FString SocketDescription = "UDP Listen Socket";
	FIPv4Endpoint LocalEndpoint;
	uint16 LocalPort = 54000;
	int32 SendSize;
	TArray<uint8> ReceivedData;

	// Remote Endpoint
	FIPv4Endpoint RemoteEndpoint;
	FIPv4Address RemoteAddress;
	uint16 RemotePort = 54001;
	int32 BufferSize;
	//FString IP = "192.168.50.232";
	FString IP = "127.0.0.1";

	ISocketSubsystem* SocketSubsystem;

	UPROPERTY(BlueprintAssignable, Category = "UDPNetworking")
		FDataReceivedDelegate OnDataReceived;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void Tick(float DeltaTime) override;

	void Listen();

	UFUNCTION(BlueprintCallable, Category = "UDPNetworking")
		bool sendMessage(FString Message);
};
