#include "cpu.h"
#include <iostream>

bool NES_cpu::getFlag(Flags F) { return (p_ & F) != 0; }

void NES_cpu::setFlag(Flags F, bool condition)
{
    p_ = (p_ & ~F) | (-(uint8_t)condition & F);
}

void NES_cpu::initialize()
{
    A_ = 0;
    X_ = 0;
    Y_ = 0;

    uint8_t low = bus_->read(0xFFFC);
    uint8_t high = bus_->read(0xFFFD);
    pc_ = (high << 8) | low;

    sp_ = 0xFD;

    // setting le flags
    setFlag(C, 0);
    setFlag(Z, 0);
    setFlag(I, 1);
    setFlag(D, 0);

    setFlag(B, 0);
    setFlag(U, 1);
    setFlag(V, 0);
    setFlag(N, 0);
}

void NES_cpu::emulateCycle()
{
    uint8_t opcode_ = bus_->read(pc_++);
}

// INSTRUCTION SET

// ugly code alert !!!
NES_cpu::Instruction NES_cpu::instructionSet[256] = {
    /*$00*/ {&NES_cpu::interruptSoftware, &NES_cpu::addrImplicit},
    /*$01*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrIndirectX},
    /*$02*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$03*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$04*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$05*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrZeroPage},
    /*$06*/ {&NES_cpu::shiftLeft, &NES_cpu::addrZeroPage},
    /*$07*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$08*/ {&NES_cpu::pushP, &NES_cpu::addrImplicit},
    /*$09*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrImmediate},
    /*$0A*/ {&NES_cpu::shiftLeft, &NES_cpu::addrAccumulator},
    /*$0B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$0C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$0D*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrAbsolute},
    /*$0E*/ {&NES_cpu::shiftLeft, &NES_cpu::addrAbsolute},
    /*$0F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$10*/ {&NES_cpu::branchIfPlus, &NES_cpu::addrRelative},
    /*$11*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrIndirectY},
    /*$12*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$13*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$14*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$15*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrZeroPageX},
    /*$16*/ {&NES_cpu::shiftLeft, &NES_cpu::addrZeroPageX},
    /*$17*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$18*/ {&NES_cpu::clearC, &NES_cpu::addrImplicit},
    /*$19*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrAbsoluteY},
    /*$1A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$1B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$1C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$1D*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrAbsoluteX},
    /*$1E*/ {&NES_cpu::shiftLeft, &NES_cpu::addrAbsoluteX},
    /*$1F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$20*/ {&NES_cpu::jumpToSubroutine, &NES_cpu::addrAbsolute},
    /*$21*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrIndirectX},
    /*$22*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$23*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$24*/ {&NES_cpu::bitTest, &NES_cpu::addrZeroPage},
    /*$25*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrZeroPage},
    /*$26*/ {&NES_cpu::rotateLeft, &NES_cpu::addrZeroPage},
    /*$27*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$28*/ {&NES_cpu::pullP, &NES_cpu::addrImplicit},
    /*$29*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrImmediate},
    /*$2A*/ {&NES_cpu::rotateLeft, &NES_cpu::addrAccumulator},
    /*$2B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$2C*/ {&NES_cpu::bitTest, &NES_cpu::addrAbsolute},
    /*$2D*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrAbsolute},
    /*$2E*/ {&NES_cpu::rotateLeft, &NES_cpu::addrAbsolute},
    /*$2F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$30*/ {&NES_cpu::branchIfMinus, &NES_cpu::addrRelative},
    /*$31*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrIndirectY},
    /*$32*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$33*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$34*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$35*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrZeroPageX},
    /*$36*/ {&NES_cpu::rotateLeft, &NES_cpu::addrZeroPageX},
    /*$37*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$38*/ {&NES_cpu::setC, &NES_cpu::addrImplicit},
    /*$39*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrAbsoluteY},
    /*$3A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$3B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$3C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$3D*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrAbsoluteX},
    /*$3E*/ {&NES_cpu::rotateLeft, &NES_cpu::addrAbsoluteX},
    /*$3F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$40*/ {&NES_cpu::returnFromInterrupt, &NES_cpu::addrImplicit},
    /*$41*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrIndirectX},
    /*$42*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$43*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$44*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$45*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrZeroPage},
    /*$46*/ {&NES_cpu::shiftRight, &NES_cpu::addrZeroPage},
    /*$47*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$48*/ {&NES_cpu::pushA, &NES_cpu::addrImplicit},
    /*$49*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrImmediate},
    /*$4A*/ {&NES_cpu::shiftRight, &NES_cpu::addrAccumulator},
    /*$4B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$4C*/ {&NES_cpu::jumpTo, &NES_cpu::addrAbsolute},
    /*$4D*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrAbsolute},
    /*$4E*/ {&NES_cpu::shiftRight, &NES_cpu::addrAbsolute},
    /*$4F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$50*/ {&NES_cpu::branchIfOverflowClear, &NES_cpu::addrRelative},
    /*$51*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrIndirectY},
    /*$52*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$53*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$54*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$55*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrZeroPageX},
    /*$56*/ {&NES_cpu::shiftRight, &NES_cpu::addrZeroPageX},
    /*$57*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$58*/ {&NES_cpu::clearI, &NES_cpu::addrImplicit},
    /*$59*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrAbsoluteY},
    /*$5A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$5B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$5C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$5D*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrAbsoluteX},
    /*$5E*/ {&NES_cpu::shiftRight, &NES_cpu::addrAbsoluteX},
    /*$5F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$60*/ {&NES_cpu::returnFromSubroutine, &NES_cpu::addrImplicit},
    /*$61*/ {&NES_cpu::addWithCarry, &NES_cpu::addrIndirectX},
    /*$62*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$63*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$64*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$65*/ {&NES_cpu::addWithCarry, &NES_cpu::addrZeroPage},
    /*$66*/ {&NES_cpu::rotateRight, &NES_cpu::addrZeroPage},
    /*$67*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$68*/ {&NES_cpu::pullA, &NES_cpu::addrImplicit},
    /*$69*/ {&NES_cpu::addWithCarry, &NES_cpu::addrImmediate},
    /*$6A*/ {&NES_cpu::rotateRight, &NES_cpu::addrAccumulator},
    /*$6B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$6C*/ {&NES_cpu::jumpTo, &NES_cpu::addrIndirect},
    /*$6D*/ {&NES_cpu::addWithCarry, &NES_cpu::addrAbsolute},
    /*$6E*/ {&NES_cpu::rotateRight, &NES_cpu::addrAbsolute},
    /*$6F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$70*/ {&NES_cpu::branchIfOverflowSet, &NES_cpu::addrRelative},
    /*$71*/ {&NES_cpu::addWithCarry, &NES_cpu::addrIndirectY},
    /*$72*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$73*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$74*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$75*/ {&NES_cpu::addWithCarry, &NES_cpu::addrZeroPageX},
    /*$76*/ {&NES_cpu::rotateRight, &NES_cpu::addrZeroPageX},
    /*$77*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$78*/ {&NES_cpu::setI, &NES_cpu::addrImplicit},
    /*$79*/ {&NES_cpu::addWithCarry, &NES_cpu::addrAbsoluteY},
    /*$7A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$7B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$7C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$7D*/ {&NES_cpu::addWithCarry, &NES_cpu::addrAbsoluteX},
    /*$7E*/ {&NES_cpu::rotateRight, &NES_cpu::addrAbsoluteX},
    /*$7F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$80*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$81*/ {&NES_cpu::storeA, &NES_cpu::addrIndirectX},
    /*$82*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$83*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$84*/ {&NES_cpu::storeY, &NES_cpu::addrZeroPage},
    /*$85*/ {&NES_cpu::storeA, &NES_cpu::addrZeroPage},
    /*$86*/ {&NES_cpu::storeX, &NES_cpu::addrZeroPage},
    /*$87*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$88*/ {&NES_cpu::decrementY, &NES_cpu::addrImplicit},
    /*$89*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$8A*/ {&NES_cpu::transferXtoA, &NES_cpu::addrImplicit},
    /*$8B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$8C*/ {&NES_cpu::storeY, &NES_cpu::addrAbsolute},
    /*$8D*/ {&NES_cpu::storeA, &NES_cpu::addrAbsolute},
    /*$8E*/ {&NES_cpu::storeX, &NES_cpu::addrAbsolute},
    /*$8F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$90*/ {&NES_cpu::branchIfCarryClear, &NES_cpu::addrRelative},
    /*$91*/ {&NES_cpu::storeA, &NES_cpu::addrIndirectY},
    /*$92*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$93*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$94*/ {&NES_cpu::storeY, &NES_cpu::addrZeroPageX},
    /*$95*/ {&NES_cpu::storeA, &NES_cpu::addrZeroPageX},
    /*$96*/ {&NES_cpu::storeX, &NES_cpu::addrZeroPageY},
    /*$97*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$98*/ {&NES_cpu::transferYtoA, &NES_cpu::addrImplicit},
    /*$99*/ {&NES_cpu::storeA, &NES_cpu::addrAbsoluteY},
    /*$9A*/ {&NES_cpu::transferXtoSP, &NES_cpu::addrImplicit},
    /*$9B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$9C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$9D*/ {&NES_cpu::storeA, &NES_cpu::addrAbsoluteX},
    /*$9E*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$9F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$A0*/ {&NES_cpu::loadY, &NES_cpu::addrImmediate},
    /*$A1*/ {&NES_cpu::loadA, &NES_cpu::addrIndirectX},
    /*$A2*/ {&NES_cpu::loadX, &NES_cpu::addrImmediate},
    /*$A3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$A4*/ {&NES_cpu::loadY, &NES_cpu::addrZeroPage},
    /*$A5*/ {&NES_cpu::loadA, &NES_cpu::addrZeroPage},
    /*$A6*/ {&NES_cpu::loadX, &NES_cpu::addrZeroPage},
    /*$A7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$A8*/ {&NES_cpu::transferAtoY, &NES_cpu::addrImplicit},
    /*$A9*/ {&NES_cpu::loadA, &NES_cpu::addrImmediate},
    /*$AA*/ {&NES_cpu::transferAtoX, &NES_cpu::addrImplicit},
    /*$AB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$AC*/ {&NES_cpu::loadY, &NES_cpu::addrAbsolute},
    /*$AD*/ {&NES_cpu::loadA, &NES_cpu::addrAbsolute},
    /*$AE*/ {&NES_cpu::loadX, &NES_cpu::addrAbsolute},
    /*$AF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$B0*/ {&NES_cpu::branchIfCarrySet, &NES_cpu::addrRelative},
    /*$B1*/ {&NES_cpu::loadA, &NES_cpu::addrIndirectY},
    /*$B2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$B3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$B4*/ {&NES_cpu::loadY, &NES_cpu::addrZeroPageX},
    /*$B5*/ {&NES_cpu::loadA, &NES_cpu::addrZeroPageX},
    /*$B6*/ {&NES_cpu::loadX, &NES_cpu::addrZeroPageY},
    /*$B7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$B8*/ {&NES_cpu::clearV, &NES_cpu::addrImplicit},
    /*$B9*/ {&NES_cpu::loadA, &NES_cpu::addrAbsoluteY},
    /*$BA*/ {&NES_cpu::transferSPtoX, &NES_cpu::addrImplicit},
    /*$BB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$BC*/ {&NES_cpu::loadY, &NES_cpu::addrAbsoluteX},
    /*$BD*/ {&NES_cpu::loadA, &NES_cpu::addrAbsoluteX},
    /*$BE*/ {&NES_cpu::loadX, &NES_cpu::addrAbsoluteY},
    /*$BF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$C0*/ {&NES_cpu::compareY, &NES_cpu::addrImmediate},
    /*$C1*/ {&NES_cpu::compareA, &NES_cpu::addrIndirectX},
    /*$C2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$C3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$C4*/ {&NES_cpu::compareY, &NES_cpu::addrZeroPage},
    /*$C5*/ {&NES_cpu::compareA, &NES_cpu::addrZeroPage},
    /*$C6*/ {&NES_cpu::decrementMemory, &NES_cpu::addrZeroPage},
    /*$C7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$C8*/ {&NES_cpu::incrementY, &NES_cpu::addrImplicit},
    /*$C9*/ {&NES_cpu::compareA, &NES_cpu::addrImmediate},
    /*$CA*/ {&NES_cpu::decrementX, &NES_cpu::addrImplicit},
    /*$CB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$CC*/ {&NES_cpu::compareY, &NES_cpu::addrAbsolute},
    /*$CD*/ {&NES_cpu::compareA, &NES_cpu::addrAbsolute},
    /*$CE*/ {&NES_cpu::decrementMemory, &NES_cpu::addrAbsolute},
    /*$CF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$D0*/ {&NES_cpu::branchIfNotEqual, &NES_cpu::addrRelative},
    /*$D1*/ {&NES_cpu::compareA, &NES_cpu::addrIndirectY},
    /*$D2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$D3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$D4*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$D5*/ {&NES_cpu::compareA, &NES_cpu::addrZeroPageX},
    /*$D6*/ {&NES_cpu::decrementMemory, &NES_cpu::addrZeroPageX},
    /*$D7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$D8*/ {&NES_cpu::clearD, &NES_cpu::addrImplicit},
    /*$D9*/ {&NES_cpu::compareA, &NES_cpu::addrAbsoluteY},
    /*$DA*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$DB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$DC*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$DD*/ {&NES_cpu::compareA, &NES_cpu::addrAbsoluteX},
    /*$DE*/ {&NES_cpu::decrementMemory, &NES_cpu::addrAbsoluteX},
    /*$DF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$E0*/ {&NES_cpu::compareX, &NES_cpu::addrImmediate},
    /*$E1*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrIndirectX},
    /*$E2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$E3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$E4*/ {&NES_cpu::compareX, &NES_cpu::addrZeroPage},
    /*$E5*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrZeroPage},
    /*$E6*/ {&NES_cpu::incrementMemory, &NES_cpu::addrZeroPage},
    /*$E7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$E8*/ {&NES_cpu::incrementX, &NES_cpu::addrImplicit},
    /*$E9*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrImmediate},
    /*$EA*/ {&NES_cpu::noOperation, &NES_cpu::addrImplicit},
    /*$EB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$EC*/ {&NES_cpu::compareX, &NES_cpu::addrAbsolute},
    /*$ED*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrAbsolute},
    /*$EE*/ {&NES_cpu::incrementMemory, &NES_cpu::addrAbsolute},
    /*$EF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},

    /*$F0*/ {&NES_cpu::branchIfEqual, &NES_cpu::addrRelative},
    /*$F1*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrIndirectY},
    /*$F2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$F3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$F4*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$F5*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrZeroPageX},
    /*$F6*/ {&NES_cpu::incrementMemory, &NES_cpu::addrZeroPageX},
    /*$F7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$F8*/ {&NES_cpu::setD, &NES_cpu::addrImplicit},
    /*$F9*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrAbsoluteY},
    /*$FA*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$FB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$FC*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
    /*$FD*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrAbsoluteX},
    /*$FE*/ {&NES_cpu::incrementMemory, &NES_cpu::addrAbsoluteX},
    /*$FF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode},
};

// Addressing modes
//// Indexed based
uint16_t NES_cpu::addrZeroPageX()
{
    uint8_t baseAddress = bus_->read(pc_++);
    uint16_t effectiveAddress = (baseAddress + X_) & 0xFF;
    return effectiveAddress;
}

uint16_t NES_cpu::addrZeroPageY()
{
    uint8_t baseAddress = bus_->read(pc_++);
    uint16_t effectiveAddress = (baseAddress + Y_) & 0xFF;
    return effectiveAddress;
}

uint16_t NES_cpu::addrAbsoluteX()
{
    uint8_t lowByte = bus_->read(pc_++);
    uint8_t highByte = bus_->read(pc_++);
    uint16_t effectiveAddress = (highByte << 8) | lowByte + X_;
    return effectiveAddress;
}

uint16_t NES_cpu::addrAbsoluteY()
{
    uint8_t lowByte = bus_->read(pc_++);
    uint8_t highByte = bus_->read(pc_++);
    uint16_t effectiveAddress = (highByte << 8) | lowByte + Y_;
    return effectiveAddress;
}

uint16_t NES_cpu::addrIndirectX()
{
    uint8_t baseAddress = bus_->read(pc_++);
    uint8_t effectiveAddressLow = bus_->read((baseAddress + X_) & 0xFF);
    uint8_t effectiveAddressHigh = bus_->read((baseAddress + X_ + 1) & 0xFF);
    uint16_t effectiveAddress = (effectiveAddressHigh << 8) | effectiveAddressLow;
    return effectiveAddress;
}

uint16_t NES_cpu::addrIndirectY()
{
    uint8_t baseAddress = bus_->read(pc_++);
    uint8_t effectiveAddressLow = bus_->read((baseAddress + Y_) & 0xFF);
    uint8_t effectiveAddressHigh = bus_->read((baseAddress + Y_ + 1) & 0xFF);
    uint16_t effectiveAddress = (effectiveAddressHigh << 8) | effectiveAddressLow;
    return effectiveAddress;
}

//// Others
uint16_t NES_cpu::addrImplicit()
{
    return 0;
}

uint16_t NES_cpu::addrAccumulator()
{
    return 0;
}

uint16_t NES_cpu::addrImmediate()
{
    return pc_++;
}

uint16_t NES_cpu::addrZeroPage()
{
    return bus_->read(pc_++);
}

uint16_t NES_cpu::addrAbsolute()
{
    uint8_t lowByte = bus_->read(pc_++);
    uint8_t highByte = bus_->read(pc_++);
    return (highByte << 8) | lowByte;
}

uint16_t NES_cpu::addrRelative()
{
    int8_t offset = (int8_t)bus_->read(pc_++);
    return pc_ + offset;
}

uint16_t NES_cpu::addrIndirect()
{
    uint8_t lowByte = bus_->read(pc_++);
    uint8_t highByte = bus_->read(pc_++);
    uint16_t address = (highByte << 8) | lowByte;
    return bus_->read(address);
}

// Access based
void NES_cpu::loadA(uint8_t memory)
{
    A_ = memory;

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000); // 7th bit
}

void NES_cpu::storeA(uint16_t address)
{
    bus_->write(address, A_);
}

void NES_cpu::loadX(uint8_t memory)
{
    X_ = memory;

    setFlag(Z, X_ == 0);
    setFlag(N, X_ & 0b10000000); // 7th bit
}

void NES_cpu::storeX(uint16_t address)
{
    bus_->write(address, X_);
}

void NES_cpu::loadY(uint8_t memory)
{
    Y_ = memory;

    setFlag(Z, Y_ == 0);
    setFlag(N, Y_ & 0b10000000); // 7th bit
}

void NES_cpu::storeY(uint16_t address)
{
    bus_->write(address, Y_);
}

// Transfer based
void NES_cpu::transferAtoX()
{
    X_ = A_;

    setFlag(Z, X_ == 0);
    setFlag(N, X_ & 0b10000000); // 7th bit
}

void NES_cpu::transferXtoA()
{
    A_ = X_;

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000); // 7th bit
}

void NES_cpu::transferAtoY()
{
    Y_ = A_;

    setFlag(Z, Y_ == 0);
    setFlag(N, Y_ & 0b10000000); // 7th bit
}

void NES_cpu::transferYtoA()
{
    A_ = Y_;

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000); // 7th bit
}

// Arithmetic based

void NES_cpu::addWithCarry(uint8_t memory)
{
    uint16_t result_ = A_ + memory + getFlag(C);

    setFlag(C, result_ > 0xFF); // overflow
    setFlag(Z, (result_ & 0xFF) == 0);
    setFlag(V, (result_ ^ A_) & (result_ ^ memory) & 0b10000000); // Check if sign bit is different between A_ and memory, and result_ and A_
    setFlag(N, result_ & 0b10000000);

    A_ = result_ & 0xFF;
}

void NES_cpu::subtractWithCarry(uint8_t memory)
{
    uint16_t result_ = A_ + ~memory + getFlag(C);

    setFlag(C, ~(result_ < 0x00)); // underflows
    setFlag(Z, (result_ & 0xFF) == 0);
    setFlag(V, (result_ ^ A_) & (result_ ^ ~memory) & 0b10000000);
    setFlag(N, result_ & 0b10000000);

    A_ = result_ & 0xFF;
}

void NES_cpu::incrementMemory(uint16_t address)
{
    uint8_t memory = bus_->read(address);
    memory++;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->write(address, memory);
}

void NES_cpu::decrementMemory(uint16_t address)
{
    uint8_t memory = bus_->read(address);
    memory--;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->write(address, memory);
}

void NES_cpu::incrementX()
{
    X_ += 1;

    setFlag(Z, X_ == 0);
    setFlag(N, X_ & 0b10000000);
}

void NES_cpu::decrementX()
{
    X_ -= 1;

    setFlag(Z, X_ == 0);
    setFlag(N, X_ & 0b10000000);
}

void NES_cpu::incrementY()
{
    Y_ += 1;

    setFlag(Z, Y_ == 0);
    setFlag(N, Y_ & 0b10000000);
}

void NES_cpu::decrementY()
{
    Y_ -= 1;

    setFlag(Z, Y_ == 0);
    setFlag(N, Y_ & 0b10000000);
}

// Shift based
void NES_cpu::shiftLeft(uint16_t address)
{
    uint8_t memory = bus_->read(address);

    setFlag(C, memory & 0b10000000); // 7th bit
    memory <<= 1;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->write(address, memory);
}

void NES_cpu::shiftRight(uint16_t address)
{
    uint8_t memory = bus_->read(address);

    setFlag(C, memory & 0b00000001); // 0th bit
    memory >>= 1;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->write(address, memory);
}

void NES_cpu::rotateLeft(uint16_t address)
{
    uint8_t memory = bus_->read(address);

    bool oldC = getFlag(C);
    setFlag(C, memory & 0b10000000); // 7th bit
    memory = (memory << 1) | oldC;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->write(address, memory);
}

void NES_cpu::rotateRight(uint16_t address)
{
    uint8_t memory = bus_->read(address);

    bool oldC = getFlag(C);
    setFlag(C, memory & 0b00000001); // 0th bit
    memory = (memory >> 1) | (oldC ? 0x80 : 0x00);

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->write(address, memory);
}

// Bitwise based
void NES_cpu::bitwiseAnd(uint8_t memory)
{
    A_ = A_ & memory;

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000);
}

void NES_cpu::bitwiseOr(uint8_t memory)
{
    A_ = A_ | memory;

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000);
}

void NES_cpu::bitwiseXor(uint8_t memory)
{
    A_ = A_ ^ memory;

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000);
}

void NES_cpu::bitTest(uint8_t memory)
{
    uint8_t result = A_ & memory;

    setFlag(Z, result == 0);
    setFlag(N, result & 0b10000000);
}

// Compare based
void NES_cpu::compareA(uint8_t memory)
{
    uint8_t result = A_ - memory;

    setFlag(C, A_ >= memory);
    setFlag(Z, A_ == memory);
    setFlag(N, result & 0b10000000);
}

void NES_cpu::compareX(uint8_t memory)
{
    uint8_t result = X_ - memory;

    setFlag(C, X_ >= memory);
    setFlag(Z, X_ == memory);
    setFlag(N, result & 0b10000000);
}

void NES_cpu::compareY(uint8_t memory)
{
    uint8_t result = Y_ - memory;

    setFlag(C, Y_ >= memory);
    setFlag(Z, Y_ == memory);
    setFlag(N, result & 0b10000000);
}

// Branch based
void NES_cpu::branchIfCarryClear(uint8_t memory)
{
    pc_ += (getFlag(C) == 0) ? memory : 0;
}

void NES_cpu::branchIfCarrySet(uint8_t memory)
{
    pc_ += (getFlag(C) == 1) ? memory : 0;
}

void NES_cpu::branchIfEqual(uint8_t memory)
{
    pc_ += (getFlag(Z) == 1) ? memory : 0;
}

void NES_cpu::branchIfNotEqual(uint8_t memory)
{
    pc_ += (getFlag(Z) == 0) ? memory : 0;
}

void NES_cpu::branchIfPlus(uint8_t memory)
{
    pc_ += (getFlag(N) == 0) ? memory : 0;
}

void NES_cpu::branchIfMinus(uint8_t memory)
{
    pc_ += (getFlag(N) == 1) ? memory : 0;
}

void NES_cpu::branchIfOverflowClear(uint8_t memory)
{
    pc_ += (getFlag(V) == 0) ? memory : 0;
}

void NES_cpu::branchIfOverflowSet(uint8_t memory)
{
    pc_ += (getFlag(V) == 1) ? memory : 0;
}

// Jump based
void NES_cpu::jumpTo(uint8_t memory)
{
    pc_ = memory;
}

void NES_cpu::jumpToSubroutine(uint8_t memory)
{
    bus_->write(0x0100 + sp_, (pc_ >> 8) & 0xFF); // push high byte
    bus_->write(0x0100 + sp_ - 1, pc_ & 0xFF);    // push low byte
    sp_ -= 2;

    pc_ = memory;
}

void NES_cpu::returnFromSubroutine()
{
    uint8_t lowByte = bus_->read(0x0100 + sp_ + 1);
    uint8_t highByte = bus_->read(0x0100 + sp_ + 2);
    sp_ += 2;

    pc_ = (highByte << 8) | lowByte + 1;
}

void NES_cpu::interruptSoftware()
{
    bus_->write(0x0100 + sp_, (pc_ >> 8) & 0xFF); // push high byte
    bus_->write(0x0100 + sp_ - 1, pc_ & 0xFF);    // push low byte
    sp_ -= 2;

    // push status flags to stack
    bus_->write(0x0100 + sp_, p_);

    pc_ = 0xFFFE;

    setFlag(I, 1); // disable interrupts
    setFlag(B, 1); // set B flag
}

void NES_cpu::returnFromInterrupt()
{
    p_ = bus_->read(0x0100 + sp_ + 1); // pop status flags from stack
    uint8_t lowByte = bus_->read(0x0100 + sp_ + 2);
    uint8_t highByte = bus_->read(0x0100 + sp_ + 3);
    sp_ += 3;

    pc_ = (highByte << 8) | lowByte;
}

// Stack based
void NES_cpu::pushA()
{
    bus_->write(0x0100 + sp_, A_);
    sp_--;
}

void NES_cpu::pullA()
{
    sp_++;
    A_ = bus_->read(0x0100 + sp_);

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000);
}

void NES_cpu::pushP()
{
    setFlag(B, 1);

    bus_->write(0x0100 + sp_, p_);
    sp_--;
}

void NES_cpu::pullP()
{
    sp_++;
    p_ = bus_->read(0x0100 + sp_);
}

void NES_cpu::transferXtoSP()
{
    sp_ = X_;
}

void NES_cpu::transferSPtoX()
{
    X_ = sp_;

    setFlag(Z, X_ == 0);
    setFlag(N, X_ & 0b10000000);
}

// Flags based
void NES_cpu::clearC() { setFlag(C, 0); }
void NES_cpu::setC() { setFlag(C, 1); }
void NES_cpu::clearI() { setFlag(I, 0); }
void NES_cpu::setI() { setFlag(I, 1); }
void NES_cpu::clearD() { setFlag(D, 0); }
void NES_cpu::setD() { setFlag(D, 1); }
void NES_cpu::clearV() { setFlag(V, 0); }

// NOP
void NES_cpu::noOperation()
{
    // Do nothing
}

// Error handling
void NES_cpu::handleInvalidOpcode() { throw std::runtime_error("Invalid opcode encountered."); }

uint16_t NES_cpu::handleInvalidAddressingMode()
{
    throw std::runtime_error("Invalid addressing mode encountered.");
    return -1;
}