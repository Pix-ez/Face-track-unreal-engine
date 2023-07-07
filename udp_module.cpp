#include "udp_module.h"
#include <string>
#include <deque>
#include <algorithm>
#include <numeric> // Added include for std::accumulate

Audp_module::Audp_module()
{
    PrimaryActorTick.bCanEverTick = true;
    Socket = nullptr;
}

Audp_module::~Audp_module()
{

}

void Audp_module::BeginPlay()
{
    Super::BeginPlay();

    SocketSubsystem = nullptr;
    if (SocketSubsystem == nullptr) SocketSubsystem = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM);

    SendSize = 2 * 1024 * 1024;
    //BufferSize = 2 * 1024 * 1024;

    LocalEndpoint = FIPv4Endpoint(FIPv4Address::Any, LocalPort);

    FIPv4Address::Parse(IP, RemoteAddress);
    RemoteEndpoint = FIPv4Endpoint(RemoteAddress, RemotePort);

    Socket = nullptr;

    if (SocketSubsystem != nullptr)
    {
        if (Socket == nullptr)
        {
            Socket = FUdpSocketBuilder(SocketDescription)
                .AsNonBlocking()
                .AsReusable()
                .BoundToEndpoint(LocalEndpoint)
                .WithReceiveBufferSize(SendSize)
                .WithReceiveBufferSize(BufferSize)
                .WithBroadcast();
        }
    }
}

void Audp_module::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    Super::EndPlay(EndPlayReason);
    SocketSubsystem->DestroySocket(Socket);
    Socket = nullptr;
    SocketSubsystem = nullptr;
}

void Audp_module::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    Listen(); // Listen for messages

    //FString t = "test222";
    //sendMessage(t); // Send Message Test
}

void Audp_module::Listen()
{
    TSharedRef<FInternetAddr> targetAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();

    uint32 Size;
    while (Socket->HasPendingData(Size))
    {
        ReceivedData.SetNumUninitialized(Size);
        int32 BytesRead = 0;
        Socket->RecvFrom(ReceivedData.GetData(), ReceivedData.Num(), BytesRead, *targetAddr);

        // Ensure null-termination of the received data
        ReceivedData.Add(0);

        // Convert the received data to FString
        FString ReceivedString(UTF8_TO_TCHAR(reinterpret_cast<const ANSICHAR*>(ReceivedData.GetData())));

        // Split the received string into center_y and center_z
        FString ParsedCenterY, ParsedCenterZ;
        ReceivedString.TrimStartAndEndInline();
        ReceivedString.Split(TEXT(" "), &ParsedCenterY, &ParsedCenterZ);

        // Convert the values to floats
        float ParsedCenterYValue = FCString::Atof(*ParsedCenterY);
        float ParsedCenterZValue = FCString::Atof(*ParsedCenterZ);

        // Check if both values are non-zero
        if (ParsedCenterYValue != 0.0f || ParsedCenterZValue != 0.0f)
        {
            // Assign the parsed values to the member variables
            float Y = ParsedCenterYValue;
            float Z = ParsedCenterZValue;

            // Debug output to verify received data
           // UE_LOG(LogTemp, Warning, TEXT("Received Data - center_y: %f, center_z: %f"), CenterY, CenterZ);

           // Declare and add the values to the lists
            std::deque<float> yList;
            std::deque<float> zList;

            yList.push_back(Y);
            zList.push_back(Z);

            // Remove elements from the lists if they exceed the maximum size
            if (yList.size() > 70)
                yList.pop_front();
            if (zList.size() > 70)
                zList.pop_front();

            // Calculate the average of the values in the lists
            float ySum = std::accumulate(yList.begin(), yList.end(), 0.0f);
            float zSum = std::accumulate(zList.begin(), zList.end(), 0.0f);
            float yAverage = ySum / yList.size();
            float zAverage = zSum / zList.size();

            CenterY = yAverage;
            CenterZ = zAverage;



            // Output the averages
            UE_LOG(LogTemp, Warning, TEXT("yList Average: %f, zList Average: %f"), CenterY, CenterZ);
        }
    }
}

bool Audp_module::sendMessage(FString Message)
{
    if (!Socket) return false;
    int32 BytesSent;

    FTimespan waitTime = FTimespan(10);

    TCHAR* serializedChar = Message.GetCharArray().GetData();
    int32 size = FCString::Strlen(serializedChar);

    bool success = Socket->SendTo((uint8*)TCHAR_TO_UTF8(serializedChar), size, BytesSent, *RemoteEndpoint.ToInternetAddr());
    UE_LOG(LogTemp, Warning, TEXT("Sent message: %s : %s : Address - %s : BytesSent - %d"), *Message, (success ? TEXT("true") : TEXT("false")), *RemoteEndpoint.ToString(), BytesSent);

    if (success && BytesSent > 0) return true;
    else return false;
}
