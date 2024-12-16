#ifndef RADAR_DATA_H
#define RADAR_DATA_H

#include <stdint.h>


struct RadarData {
// Frame Header
    uint16_t syncHeader[4];       // Sync Header: 0x0102, 0x0304, 0x0506, 0x0708
    uint32_t softwareVersion;     // Software version number
    uint32_t totalPacketLength;   // Total packet length (fixed at 96 bytes)
    uint32_t hardwareVersion;     // Hardware version number
    uint32_t frameNumber;         // Current frame sequence number
    uint32_t cpuTime;             // CPU time
    uint32_t humanDetected;       // Human detection status: 0 (no human), 1 (human detected)
    uint32_t outputStructureCount;// Number of output structures (always 1)
    uint32_t reserved;            // Reserved field (always 0)

    // Output Structure
    uint32_t structureType;       // Structure type (6)
    uint32_t structureLength;     // Structure length (0x14 or 20 bytes)

    // Measurements
    float maxRCS;                 // Maximum Radar Cross Section (RCS) or echo strength
    float heartRateEst;           // Heart rate estimation output
    float breathingRateEst;       // Breathing rate estimation output
    float sumEnergyBreathWfm;     // Total energy of breathing waveform (decays when no human is detected)
    float sumEnergyHeartWfm;      // Total energy of heart waveform (decays when no human is detected)
    float range;                  // Range of target object
};



#endif // RADAR_DATA_H
