/**
 * Liverpool Graphics Controller (GC/Starsha) PCI device.
 *
 * Copyright 2017-2021. Orbital project.
 * Released under MIT license. Read LICENSE for more details.
 *
 * Authors:
 * - Alexandro Sanchez Bach <alexandro@phi.nz>
 */

#pragma once

#include <orbital/core.h>
#include <orbital/hardware/liverpool/amd_ucode.h>

 // Engines
#include "gca/gfx.h"
#include "gmc/gmc.h"
#include "oss/ih.h"
#include "sam/sam.h"
#include "smu/smu.h"

#include <array>
#include <memory>

constexpr auto LIVERPOOL_GC_DEV = 0x1;
constexpr auto LIVERPOOL_GC_FNC = 0x0;
constexpr auto LIVERPOOL_GC_VID = static_cast<PCIVendorId>(0x1002);
constexpr auto LIVERPOOL_GC_DID = static_cast<PCIDeviceId>(0x9920);

struct LiverpoolGCDeviceConfig : PCIeDeviceConfig {
    GfxDeviceConfig gfx;

    LiverpoolGCDeviceConfig(PCI_DF df = PCI_DF(LIVERPOOL_GC_DEV, LIVERPOOL_GC_FNC))
        : PCIeDeviceConfig(df, LIVERPOOL_GC_VID, LIVERPOOL_GC_DID, 0x0, PCI_CLASS_DISPLAY_VGA) {
    }
};

class LiverpoolGCDevice final : public PCIeDevice {
public:
    LiverpoolGCDevice(PCIeBus* bus, const LiverpoolGCDeviceConfig& config = {});
    ~LiverpoolGCDevice();

    // Device interface
    void reset() override;

    const auto& get_mmio() const noexcept {
        return mmio;
    }

    SmuDevice* get_smu() {
        return &smu;
    }

private:
    MemorySpace* space_bar0;
    MemorySpace* space_bar2;
    MemorySpace* space_pio;
    MemorySpace* space_mmio;

    // State
    std::array<U32, 0x10000> mmio;
    AmdUcode<0x2000> sdma0_ucode;
    AmdUcode<0x2000> sdma1_ucode;

    U64 bar0_read(U64 addr, U64 size);
    void bar0_write(U64 addr, U64 value, U64 size);

    U64 bar2_read(U64 addr, U64 size);
    void bar2_write(U64 addr, U64 value, U64 size);

    U64 pio_read(U64 addr, U64 size);
    void pio_write(U64 addr, U64 value, U64 size);

    U64 mmio_read(U64 addr, U64 size);
    void mmio_write(U64 addr, U64 value, U64 size);

    // Engines
    GmcDevice gmc;
    IhDevice ih;
    SmuDevice smu;
    SamDevice sam;
    GfxDevice gfx;
};
