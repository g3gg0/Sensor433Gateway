
/*
    from https://github.com/HectorMalot/ecur/blob/f094b3ba8f7cbf15c183e7061e49ee96afbc9994/ecur.go

    NewECUInfo parses the ECUInfo call into a struct
    Decoded explanation
    ---------------------
    0- 2 : APS          = Mark start of datastream
    3- 4 : 11           = Answer notation
    4- 8 : 0094         = Datalength
    9-12 : 0001         = commandnumber
    13-24 : 216000026497 = ECU_R nummer
    25-26 : 01           = number of inverters online
    27-30 : 42896        = Lifetime energy (kWh)/10
    31-34 : 00 00 00 00  = LastSystemPower kW/100
    35-38 : 202          = CurrentDayEnergy (/100)
    39-45 : 7xd0         = LastTimeConnectEMA
    46-47 : 8            = number of inverters registered
    48-49 : 0            = number of inverters online
    50-51 : 10           = EcuChannel
    52-54 : 014          = VersionLEN => VL
    55-55+VL          : ECU_R_1.2.13 = Version
    56+VL-57+VL       : 009          = TimeZoneLen => TL
    58+VL-57+VL+TL    : Etc/GMT-8    = Timezone server always indicated at -8 hours
    58+VL+TL-63+VL+TL : 80 97 1b 01 5d 1e = EthernetMAC
    64+VL+TL-69+VL+TL : 00 00 00 00 00 00 = WirelessMAC //Shoud be but there is a bugin firmware
    70+VL+TL-73+VL+TL : END\n             = SignatureStop Marks end of datastream
*/
typedef struct __attribute__((packed))
{
    uint8_t ecuid[12];
    uint16_t inverters_online;
    uint32_t energy_lifetime;
    uint32_t power_current;
    uint32_t energy_day;
    uint8_t last_time_connected[7];
    uint16_t inverters_registered;
    uint16_t inverters_online_;
    uint16_t ecu_channel;
} t_ecuinfo;

typedef struct __attribute__((packed))
{
    uint8_t timestamp[7];
    uint8_t uid[6];
    uint16_t unknown;
    uint8_t model;
    uint16_t frequency;
    uint16_t temperature;
    uint16_t power_a;
    uint16_t voltage_a;
    uint16_t power_b;
    uint16_t voltage_b;
} t_ecuinfoinverter;

typedef struct __attribute__((packed))
{
    uint8_t unk[4];
    uint16_t packets;
    t_ecuinfoinverter inverter;
} t_ecudetailed;
