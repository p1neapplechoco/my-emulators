#include "cpu.h"
#include "bus.h"
#include <iostream>
#include <bitset>

// MISCs
void NES_cpu::setBus(NES_bus &bus) { bus_ = &bus; }

// Flag based operations
bool NES_cpu::getFlag(Flags F) { return (p_ & F) != 0; }

void NES_cpu::setFlag(Flags F, bool condition) { p_ = (p_ & ~F) | (-(uint8_t)condition & F); }

void NES_cpu::initialize()
{
    A_ = 0;
    X_ = 0;
    Y_ = 0;

    uint8_t low = bus_->readCPU(0xFFFC);
    uint8_t high = bus_->readCPU(0xFFFD);
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

    cycle_ = 0;

    std::cout << "CPU initialized." << std::endl;
    printState();
    std::cout << "===============================" << std::endl;
}

uint8_t NES_cpu::emulateCycle()
{
    // printState();
    uint8_t opcode = bus_->readCPU(pc_++);
    // std::cout << "opcode: " << std::hex << (int)opcode << std::dec << std::endl;

    Instruction instruction = instructionSet[opcode];

    uint16_t address = (this->*instruction.addressingMode)();

    if (std::holds_alternative<noArgInstr>(instruction.operation))
    {
        auto operation = std::get<noArgInstr>(instruction.operation);
        (this->*operation)();
    }
    else if (std::holds_alternative<byteInstr>(instruction.operation))
    {
        auto operation = std::get<byteInstr>(instruction.operation);
        (this->*operation)(bus_->readCPU(address));
    }
    else
    {
        auto operation = std::get<addrInstr>(instruction.operation);
        (this->*operation)(address);
    }

    cycle_ += instruction.cycles;

    return instruction.cycles;
}

void NES_cpu::reset()
{
    pc_ = (bus_->readCPU(0xFFFD) << 8) | bus_->readCPU(0xFFFC);
    sp_ -= 3;
    setFlag(I, 1);

    cycle_ = 0;
}

void NES_cpu::nmi()
{
    /*
    When the CPU checks for an NMI and finds that the flip-flop is set, the CPU performs the following actions in order:
        1. Push the return address high byte onto the stack.
        2. Push the return address low byte onto the stack.
        3. Push the processor status register onto the stack. Note that the B flag will be set to 0.
        4. Read the NMI handler address from $FFFA-$FFFB.
        5. Clear the NMI handler flip-flop.
        6. Set the program counter to the address read, jumping to the NMI handler.
    */
    bus_->writeCPU(0x0100 + sp_--, (pc_ >> 8) & 0xFF);          // push high byte of PC
    bus_->writeCPU(0x0100 + sp_--, pc_ & 0xFF);                 // push low byte of PC
    setFlag(B, 0);                                              // set B flag to 0
    bus_->writeCPU(0x0100 + sp_--, p_);                         // push processor status register
    setFlag(I, 1);                                              // disable interrupt
    pc_ = (bus_->readCPU(0xFFFB) << 8) | bus_->readCPU(0xFFFA); // read NMI handler address
    cycle_ += 7;                                                // NMI takes 7 cycles

    std::cout << "NMI triggered. Jumping to address: " << std::hex << pc_ << std::dec << std::endl;
    printState();
    // throw std::runtime_error("NMI triggered. Halting CPU for debugging.");
}

// INSTRUCTION SET

// ugly code alert !!!
NES_cpu::Instruction NES_cpu::instructionSet[256] = {
    /*$00*/ {&NES_cpu::interruptSoftware, &NES_cpu::addrImplicit, 7},
    /*$01*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrIndirectX, 6},
    /*$02*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$03*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$04*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$05*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrZeroPage, 3},
    /*$06*/ {&NES_cpu::shiftLeft, &NES_cpu::addrZeroPage, 5},
    /*$07*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$08*/ {&NES_cpu::pushP, &NES_cpu::addrImplicit, 3},
    /*$09*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrImmediate, 2},
    /*$0A*/ {&NES_cpu::shiftLeft, &NES_cpu::addrAccumulator, 2},
    /*$0B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$0C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$0D*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrAbsolute, 4},
    /*$0E*/ {&NES_cpu::shiftLeft, &NES_cpu::addrAbsolute, 6},
    /*$0F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$10*/ {&NES_cpu::branchIfPlus, &NES_cpu::addrRelative, 2},
    /*$11*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrIndirectY, 5},
    /*$12*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$13*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$14*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$15*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrZeroPageX, 4},
    /*$16*/ {&NES_cpu::shiftLeft, &NES_cpu::addrZeroPageX, 6},
    /*$17*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$18*/ {&NES_cpu::clearC, &NES_cpu::addrImplicit, 2},
    /*$19*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrAbsoluteY, 4},
    /*$1A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$1B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$1C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$1D*/ {&NES_cpu::bitwiseOr, &NES_cpu::addrAbsoluteX, 4},
    /*$1E*/ {&NES_cpu::shiftLeft, &NES_cpu::addrAbsoluteX, 6},
    /*$1F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$20*/ {&NES_cpu::jumpToSubroutine, &NES_cpu::addrAbsolute, 6},
    /*$21*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrIndirectX, 6},
    /*$22*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$23*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$24*/ {&NES_cpu::bitTest, &NES_cpu::addrZeroPage, 3},
    /*$25*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrZeroPage, 3},
    /*$26*/ {&NES_cpu::rotateLeft, &NES_cpu::addrZeroPage, 5},
    /*$27*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$28*/ {&NES_cpu::pullP, &NES_cpu::addrImplicit, 4},
    /*$29*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrImmediate, 2},
    /*$2A*/ {&NES_cpu::rotateLeft, &NES_cpu::addrAccumulator, 2},
    /*$2B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$2C*/ {&NES_cpu::bitTest, &NES_cpu::addrAbsolute, 4},
    /*$2D*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrAbsolute, 4},
    /*$2E*/ {&NES_cpu::rotateLeft, &NES_cpu::addrAbsolute, 6},
    /*$2F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$30*/ {&NES_cpu::branchIfMinus, &NES_cpu::addrRelative, 2},
    /*$31*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrIndirectY, 5},
    /*$32*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$33*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$34*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$35*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrZeroPageX, 4},
    /*$36*/ {&NES_cpu::rotateLeft, &NES_cpu::addrZeroPageX, 6},
    /*$37*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$38*/ {&NES_cpu::setC, &NES_cpu::addrImplicit, 2},
    /*$39*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrAbsoluteY, 4},
    /*$3A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$3B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$3C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$3D*/ {&NES_cpu::bitwiseAnd, &NES_cpu::addrAbsoluteX, 4},
    /*$3E*/ {&NES_cpu::rotateLeft, &NES_cpu::addrAbsoluteX, 4},
    /*$3F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$40*/ {&NES_cpu::returnFromInterrupt, &NES_cpu::addrImplicit, 6},
    /*$41*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrIndirectX, 6},
    /*$42*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$43*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$44*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$45*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrZeroPage, 3},
    /*$46*/ {&NES_cpu::shiftRight, &NES_cpu::addrZeroPage, 5},
    /*$47*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$48*/ {&NES_cpu::pushA, &NES_cpu::addrImplicit, 3},
    /*$49*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrImmediate, 2},
    /*$4A*/ {&NES_cpu::shiftRight, &NES_cpu::addrAccumulator, 2},
    /*$4B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$4C*/ {&NES_cpu::jumpTo, &NES_cpu::addrAbsolute, 3},
    /*$4D*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrAbsolute, 4},
    /*$4E*/ {&NES_cpu::shiftRight, &NES_cpu::addrAbsolute, 4},
    /*$4F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$50*/ {&NES_cpu::branchIfOverflowClear, &NES_cpu::addrRelative, 2},
    /*$51*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrIndirectY, 5},
    /*$52*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$53*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$54*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$55*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrZeroPageX, 4},
    /*$56*/ {&NES_cpu::shiftRight, &NES_cpu::addrZeroPageX, 6},
    /*$57*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$58*/ {&NES_cpu::clearI, &NES_cpu::addrImplicit, 2},
    /*$59*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrAbsoluteY, 4},
    /*$5A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$5B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$5C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$5D*/ {&NES_cpu::bitwiseXor, &NES_cpu::addrAbsoluteX, 4},
    /*$5E*/ {&NES_cpu::shiftRight, &NES_cpu::addrAbsoluteX, 4},
    /*$5F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$60*/ {&NES_cpu::returnFromSubroutine, &NES_cpu::addrImplicit, 6},
    /*$61*/ {&NES_cpu::addWithCarry, &NES_cpu::addrIndirectX, 6},
    /*$62*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$63*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 8},
    /*$64*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$65*/ {&NES_cpu::addWithCarry, &NES_cpu::addrZeroPage, 3},
    /*$66*/ {&NES_cpu::rotateRight, &NES_cpu::addrZeroPage, 5},
    /*$67*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$68*/ {&NES_cpu::pullA, &NES_cpu::addrImplicit, 4},
    /*$69*/ {&NES_cpu::addWithCarry, &NES_cpu::addrImmediate, 2},
    /*$6A*/ {&NES_cpu::rotateRight, &NES_cpu::addrAccumulator, 2},
    /*$6B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$6C*/ {&NES_cpu::jumpTo, &NES_cpu::addrIndirect, 3},
    /*$6D*/ {&NES_cpu::addWithCarry, &NES_cpu::addrAbsolute, 4},
    /*$6E*/ {&NES_cpu::rotateRight, &NES_cpu::addrAbsolute, 4},
    /*$6F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$70*/ {&NES_cpu::branchIfOverflowSet, &NES_cpu::addrRelative, 2},
    /*$71*/ {&NES_cpu::addWithCarry, &NES_cpu::addrIndirectY, 5},
    /*$72*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$73*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$74*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$75*/ {&NES_cpu::addWithCarry, &NES_cpu::addrZeroPageX, 4},
    /*$76*/ {&NES_cpu::rotateRight, &NES_cpu::addrZeroPageX, 6},
    /*$77*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$78*/ {&NES_cpu::setI, &NES_cpu::addrImplicit, 2},
    /*$79*/ {&NES_cpu::addWithCarry, &NES_cpu::addrAbsoluteY, 4},
    /*$7A*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$7B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$7C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$7D*/ {&NES_cpu::addWithCarry, &NES_cpu::addrAbsoluteX, 4},
    /*$7E*/ {&NES_cpu::rotateRight, &NES_cpu::addrAbsoluteX, 4},
    /*$7F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$80*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$81*/ {&NES_cpu::storeA, &NES_cpu::addrIndirectX, 6},
    /*$82*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$83*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$84*/ {&NES_cpu::storeY, &NES_cpu::addrZeroPage, 3},
    /*$85*/ {&NES_cpu::storeA, &NES_cpu::addrZeroPage, 3},
    /*$86*/ {&NES_cpu::storeX, &NES_cpu::addrZeroPage, 3},
    /*$87*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$88*/ {&NES_cpu::decrementY, &NES_cpu::addrImplicit, 2},
    /*$89*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$8A*/ {&NES_cpu::transferXtoA, &NES_cpu::addrImplicit, 2},
    /*$8B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$8C*/ {&NES_cpu::storeY, &NES_cpu::addrAbsolute, 4},
    /*$8D*/ {&NES_cpu::storeA, &NES_cpu::addrAbsolute, 4},
    /*$8E*/ {&NES_cpu::storeX, &NES_cpu::addrAbsolute, 4},
    /*$8F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$90*/ {&NES_cpu::branchIfCarryClear, &NES_cpu::addrRelative, 2},
    /*$91*/ {&NES_cpu::storeA, &NES_cpu::addrIndirectY, 5},
    /*$92*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$93*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$94*/ {&NES_cpu::storeY, &NES_cpu::addrZeroPageX, 4},
    /*$95*/ {&NES_cpu::storeA, &NES_cpu::addrZeroPageX, 4},
    /*$96*/ {&NES_cpu::storeX, &NES_cpu::addrZeroPageY, 4},
    /*$97*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$98*/ {&NES_cpu::transferYtoA, &NES_cpu::addrImplicit, 2},
    /*$99*/ {&NES_cpu::storeA, &NES_cpu::addrAbsoluteY, 4},
    /*$9A*/ {&NES_cpu::transferXtoSP, &NES_cpu::addrImplicit, 2},
    /*$9B*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$9C*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$9D*/ {&NES_cpu::storeA, &NES_cpu::addrAbsoluteX, 4},
    /*$9E*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$9F*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$A0*/ {&NES_cpu::loadY, &NES_cpu::addrImmediate, 2},
    /*$A1*/ {&NES_cpu::loadA, &NES_cpu::addrIndirectX, 6},
    /*$A2*/ {&NES_cpu::loadX, &NES_cpu::addrImmediate, 2},
    /*$A3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$A4*/ {&NES_cpu::loadY, &NES_cpu::addrZeroPage, 3},
    /*$A5*/ {&NES_cpu::loadA, &NES_cpu::addrZeroPage, 3},
    /*$A6*/ {&NES_cpu::loadX, &NES_cpu::addrZeroPage, 3},
    /*$A7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$A8*/ {&NES_cpu::transferAtoY, &NES_cpu::addrImplicit, 2},
    /*$A9*/ {&NES_cpu::loadA, &NES_cpu::addrImmediate, 2},
    /*$AA*/ {&NES_cpu::transferAtoX, &NES_cpu::addrImplicit, 2},
    /*$AB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$AC*/ {&NES_cpu::loadY, &NES_cpu::addrAbsolute, 4},
    /*$AD*/ {&NES_cpu::loadA, &NES_cpu::addrAbsolute, 4},
    /*$AE*/ {&NES_cpu::loadX, &NES_cpu::addrAbsolute, 4},
    /*$AF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$B0*/ {&NES_cpu::branchIfCarrySet, &NES_cpu::addrRelative, 2},
    /*$B1*/ {&NES_cpu::loadA, &NES_cpu::addrIndirectY, 5},
    /*$B2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$B3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$B4*/ {&NES_cpu::loadY, &NES_cpu::addrZeroPageX, 4},
    /*$B5*/ {&NES_cpu::loadA, &NES_cpu::addrZeroPageX, 4},
    /*$B6*/ {&NES_cpu::loadX, &NES_cpu::addrZeroPageY, 4},
    /*$B7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$B8*/ {&NES_cpu::clearV, &NES_cpu::addrImplicit, 2},
    /*$B9*/ {&NES_cpu::loadA, &NES_cpu::addrAbsoluteY, 4},
    /*$BA*/ {&NES_cpu::transferSPtoX, &NES_cpu::addrImplicit, 2},
    /*$BB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$BC*/ {&NES_cpu::loadY, &NES_cpu::addrAbsoluteX, 4},
    /*$BD*/ {&NES_cpu::loadA, &NES_cpu::addrAbsoluteX, 4},
    /*$BE*/ {&NES_cpu::loadX, &NES_cpu::addrAbsoluteY, 4},
    /*$BF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$C0*/ {&NES_cpu::compareY, &NES_cpu::addrImmediate, 2},
    /*$C1*/ {&NES_cpu::compareA, &NES_cpu::addrIndirectX, 6},
    /*$C2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$C3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$C4*/ {&NES_cpu::compareY, &NES_cpu::addrZeroPage, 3},
    /*$C5*/ {&NES_cpu::compareA, &NES_cpu::addrZeroPage, 3},
    /*$C6*/ {&NES_cpu::decrementMemory, &NES_cpu::addrZeroPage, 5},
    /*$C7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$C8*/ {&NES_cpu::incrementY, &NES_cpu::addrImplicit, 2},
    /*$C9*/ {&NES_cpu::compareA, &NES_cpu::addrImmediate, 2},
    /*$CA*/ {&NES_cpu::decrementX, &NES_cpu::addrImplicit, 2},
    /*$CB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$CC*/ {&NES_cpu::compareY, &NES_cpu::addrAbsolute, 4},
    /*$CD*/ {&NES_cpu::compareA, &NES_cpu::addrAbsolute, 4},
    /*$CE*/ {&NES_cpu::decrementMemory, &NES_cpu::addrAbsolute, 6},
    /*$CF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$D0*/ {&NES_cpu::branchIfNotEqual, &NES_cpu::addrRelative, 2},
    /*$D1*/ {&NES_cpu::compareA, &NES_cpu::addrIndirectY, 6},
    /*$D2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$D3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$D4*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$D5*/ {&NES_cpu::compareA, &NES_cpu::addrZeroPageX, 4},
    /*$D6*/ {&NES_cpu::decrementMemory, &NES_cpu::addrZeroPageX, 6},
    /*$D7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$D8*/ {&NES_cpu::clearD, &NES_cpu::addrImplicit, 2},
    /*$D9*/ {&NES_cpu::compareA, &NES_cpu::addrAbsoluteY, 4},
    /*$DA*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$DB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$DC*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$DD*/ {&NES_cpu::compareA, &NES_cpu::addrAbsoluteX, 4},
    /*$DE*/ {&NES_cpu::decrementMemory, &NES_cpu::addrAbsoluteX, 6},
    /*$DF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$E0*/ {&NES_cpu::compareX, &NES_cpu::addrImmediate, 2},
    /*$E1*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrIndirectX, 6},
    /*$E2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$E3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$E4*/ {&NES_cpu::compareX, &NES_cpu::addrZeroPage, 3},
    /*$E5*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrZeroPage, 3},
    /*$E6*/ {&NES_cpu::incrementMemory, &NES_cpu::addrZeroPage, 5},
    /*$E7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$E8*/ {&NES_cpu::incrementX, &NES_cpu::addrImplicit, 2},
    /*$E9*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrImmediate, 2},
    /*$EA*/ {&NES_cpu::noOperation, &NES_cpu::addrImplicit, 2},
    /*$EB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$EC*/ {&NES_cpu::compareX, &NES_cpu::addrAbsolute, 4},
    /*$ED*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrAbsolute, 4},
    /*$EE*/ {&NES_cpu::incrementMemory, &NES_cpu::addrAbsolute, 6},
    /*$EF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},

    /*$F0*/ {&NES_cpu::branchIfEqual, &NES_cpu::addrRelative, 2},
    /*$F1*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrIndirectY, 5},
    /*$F2*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$F3*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$F4*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$F5*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrZeroPageX, 4},
    /*$F6*/ {&NES_cpu::incrementMemory, &NES_cpu::addrZeroPageX, 6},
    /*$F7*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$F8*/ {&NES_cpu::setD, &NES_cpu::addrImplicit, 2},
    /*$F9*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrAbsoluteY, 4},
    /*$FA*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$FB*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$FC*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
    /*$FD*/ {&NES_cpu::subtractWithCarry, &NES_cpu::addrAbsoluteX, 4},
    /*$FE*/ {&NES_cpu::incrementMemory, &NES_cpu::addrAbsoluteX, 6},
    /*$FF*/ {&NES_cpu::handleInvalidOpcode, &NES_cpu::handleInvalidAddressingMode, 2},
};

// Addressing modes
//// Indexed based
uint16_t NES_cpu::addrZeroPageX()
{
    uint8_t baseAddress = bus_->readCPU(pc_++);
    uint16_t effectiveAddress = (baseAddress + X_) & 0xFF;
    return effectiveAddress;
}

uint16_t NES_cpu::addrZeroPageY()
{
    uint8_t baseAddress = bus_->readCPU(pc_++);
    uint16_t effectiveAddress = (baseAddress + Y_) & 0xFF;
    return effectiveAddress;
}

uint16_t NES_cpu::addrAbsoluteX()
{
    uint8_t lowByte = bus_->readCPU(pc_++);
    uint8_t highByte = bus_->readCPU(pc_++);
    uint16_t effectiveAddress = ((highByte << 8) | lowByte) + X_;
    return effectiveAddress;
}

uint16_t NES_cpu::addrAbsoluteY()
{
    uint8_t lowByte = bus_->readCPU(pc_++);
    uint8_t highByte = bus_->readCPU(pc_++);
    uint16_t effectiveAddress = ((highByte << 8) | lowByte) + Y_;
    return effectiveAddress;
}

uint16_t NES_cpu::addrIndirectX()
{
    uint8_t baseAddress = bus_->readCPU(pc_++);
    uint8_t effectiveAddressLow = bus_->readCPU((baseAddress + X_) & 0xFF);
    uint8_t effectiveAddressHigh = bus_->readCPU((baseAddress + X_ + 1) & 0xFF);
    uint16_t effectiveAddress = (effectiveAddressHigh << 8) | effectiveAddressLow;
    return effectiveAddress;
}

uint16_t NES_cpu::addrIndirectY()
{
    uint8_t baseAddress = bus_->readCPU(pc_++);
    uint8_t effectiveAddressLow = bus_->readCPU((baseAddress + Y_) & 0xFF);
    uint8_t effectiveAddressHigh = bus_->readCPU((baseAddress + Y_ + 1) & 0xFF);
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
    return bus_->readCPU(pc_++);
}

uint16_t NES_cpu::addrAbsolute()
{
    uint8_t lowByte = bus_->readCPU(pc_++);
    uint8_t highByte = bus_->readCPU(pc_++);
    return (highByte << 8) | lowByte;
}

uint16_t NES_cpu::addrRelative()
{
    int8_t offset = (int8_t)bus_->readCPU(pc_++);
    return pc_ + offset;
}

uint16_t NES_cpu::addrIndirect()
{
    uint8_t lowByte = bus_->readCPU(pc_++);
    uint8_t highByte = bus_->readCPU(pc_++);
    uint16_t address = (highByte << 8) | lowByte;
    return bus_->readCPU(address);
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
    bus_->writeCPU(address, A_);
}

void NES_cpu::loadX(uint8_t memory)
{
    X_ = memory;

    setFlag(Z, X_ == 0);
    setFlag(N, X_ & 0b10000000); // 7th bit
}

void NES_cpu::storeX(uint16_t address)
{
    bus_->writeCPU(address, X_);
}

void NES_cpu::loadY(uint8_t memory)
{
    Y_ = memory;

    setFlag(Z, Y_ == 0);
    setFlag(N, Y_ & 0b10000000); // 7th bit
}

void NES_cpu::storeY(uint16_t address)
{
    bus_->writeCPU(address, Y_);
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

    setFlag(C, !(result_ < 0x00)); // underflows
    setFlag(Z, (result_ & 0xFF) == 0);
    setFlag(V, (result_ ^ A_) & (result_ ^ ~memory) & 0b10000000);
    setFlag(N, result_ & 0b10000000);

    A_ = result_ & 0xFF;
}

void NES_cpu::incrementMemory(uint16_t address)
{
    uint8_t memory = bus_->readCPU(address);
    memory++;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->writeCPU(address, memory);
}

void NES_cpu::decrementMemory(uint16_t address)
{
    uint8_t memory = bus_->readCPU(address);
    memory--;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->writeCPU(address, memory);
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
    uint8_t memory = bus_->readCPU(address);

    setFlag(C, memory & 0b10000000); // 7th bit
    memory <<= 1;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->writeCPU(address, memory);
}

void NES_cpu::shiftRight(uint16_t address)
{
    uint8_t memory = bus_->readCPU(address);

    setFlag(C, memory & 0b00000001); // 0th bit
    memory >>= 1;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->writeCPU(address, memory);
}

void NES_cpu::rotateLeft(uint16_t address)
{
    uint8_t memory = bus_->readCPU(address);

    bool oldC = getFlag(C);
    setFlag(C, memory & 0b10000000); // 7th bit
    memory = (memory << 1) | oldC;

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->writeCPU(address, memory);
}

void NES_cpu::rotateRight(uint16_t address)
{
    uint8_t memory = bus_->readCPU(address);

    bool oldC = getFlag(C);
    setFlag(C, memory & 0b00000001); // 0th bit
    memory = (memory >> 1) | (oldC ? 0x80 : 0x00);

    setFlag(Z, memory == 0);
    setFlag(N, memory & 0b10000000);

    bus_->writeCPU(address, memory);
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
void NES_cpu::branchIfCarryClear(uint16_t address)
{
    if (getFlag(C) == 0)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

void NES_cpu::branchIfCarrySet(uint16_t address)
{
    if (getFlag(C) == 1)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

void NES_cpu::branchIfEqual(uint16_t address)
{
    if (getFlag(Z) == 1)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

void NES_cpu::branchIfNotEqual(uint16_t address)
{
    if (getFlag(Z) == 0)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

void NES_cpu::branchIfPlus(uint16_t address)
{
    if (getFlag(N) == 0)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

void NES_cpu::branchIfMinus(uint16_t address)
{
    if (getFlag(N) == 1)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

void NES_cpu::branchIfOverflowClear(uint16_t address)
{
    if (getFlag(V) == 0)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

void NES_cpu::branchIfOverflowSet(uint16_t address)
{
    if (getFlag(V) == 1)
    {
        pc_ = address;

        if ((pc_ & 0xFF00) != (address & 0xFF00))
            cycle_++;
    }
}

// Jump based
void NES_cpu::jumpTo(uint16_t address)
{
    pc_ = address;
}

void NES_cpu::jumpToSubroutine(uint16_t address)
{
    /*
    JSR - Jump to Subroutine

    push PC + 2 high byte to stack
    push PC + 2 low byte to stack
    PC = memory

    JSR pushes the current program counter to the stack and then sets the program counter to a new value. This allows code to call a function and return with RTS back to the instruction after the JSR.

    Notably, the return address on the stack points 1 byte before the start of the next instruction, rather than directly at the instruction. This is because RTS increments the program counter before the next instruction is fetched. This differs from the return address pushed by interrupts and used by RTI, which points directly at the next instruction.
    */

    uint16_t returnAddress = pc_ - 1; // Store the return address (current PC - 1)

    bus_->writeCPU(0x0100 + sp_, (returnAddress >> 8) & 0xFF); // push high byte
    bus_->writeCPU(0x0100 + sp_ - 1, returnAddress & 0xFF);    // push low byte
    sp_ -= 2;

    pc_ = address;
}

void NES_cpu::returnFromSubroutine()
{
    /*
    RTS - Return from Subroutine

    pull PC low byte from stack
    pull PC high byte from stack
    PC = PC + 1

    RTS pulls an address from the stack into the program counter and then increments the program counter. It is normally used at the end of a function to return to the instruction after the JSR that called the function. However, RTS is also sometimes used to implement jump tables (see Jump table and RTS Trick).
    */

    uint8_t lowByte = bus_->readCPU(0x0100 + sp_ + 1);
    uint8_t highByte = bus_->readCPU(0x0100 + sp_ + 2);
    sp_ += 2;

    pc_ = ((highByte << 8) | lowByte) + 1;
}

void NES_cpu::interruptSoftware()
{
    /*
    BRK - Break (software IRQ)

    push PC + 2 high byte to stack
    push PC + 2 low byte to stack
    push NV11DIZC flags to stack
    PC = ($FFFE)

    BRK triggers an interrupt request (IRQ). IRQs are normally triggered by external hardware, and BRK is the only way to do it in software. Like a typical IRQ, it pushes the current program counter and processor flags to the stack, sets the interrupt disable flag, and jumps to the IRQ handler. Unlike a typical IRQ, it sets the break flag in the flags byte that is pushed to the stack (like PHP) and it triggers an interrupt even if the interrupt disable flag is set. Notably, the return address that is pushed to the stack skips the byte after the BRK opcode. For this reason, BRK is often considered a 2-byte instruction with an unused immediate.

    Unfortunately, a 6502 bug allows the BRK IRQ to be overridden by an NMI occurring at the same time. In this case, only the NMI handler is called; the IRQ handler is skipped. However, the break flag is still set in the flags byte pushed to the stack, so the NMI handler can detect that this occurred (albeit slowly) by checking this flag.

    Because BRK uses the value $00, any byte in a programmable ROM can be overwritten with a BRK instruction to send execution to an IRQ handler. This is useful for patching one-time programmable ROMs. BRK can also be used as a system call mechanism, and the unused byte can be used by software as an argument (although it is inconvenient to access). In the context of NES games, BRK is often most useful as a crash handler, where the unused program space is filled with $00 and the IRQ handler displays debugging information or otherwise handles the crash in a clean way.
    */

    uint16_t returnAddress = pc_ + 1; // Store the return address (current PC + 1)

    bus_->writeCPU(0x0100 + sp_, (returnAddress >> 8) & 0xFF); // push high byte
    bus_->writeCPU(0x0100 + sp_ - 1, returnAddress & 0xFF);    // push low byte
    bus_->writeCPU(0x0100 + sp_ - 2, p_);                      // push status flags
    sp_ -= 3;

    setFlag(I, 1); // Set the interrupt disable flag
    setFlag(B, 1); // Set the break flag
}

void NES_cpu::returnFromInterrupt()
{
    p_ = bus_->readCPU(0x0100 + sp_ + 1); // pop status flags from stack
    uint8_t lowByte = bus_->readCPU(0x0100 + sp_ + 2);
    uint8_t highByte = bus_->readCPU(0x0100 + sp_ + 3);
    sp_ += 3;

    pc_ = (highByte << 8) | lowByte;
}

// Stack based
void NES_cpu::pushA()
{
    bus_->writeCPU(0x0100 + sp_, A_);
    sp_--;
}

void NES_cpu::pullA()
{
    sp_++;
    A_ = bus_->readCPU(0x0100 + sp_);

    setFlag(Z, A_ == 0);
    setFlag(N, A_ & 0b10000000);
}

void NES_cpu::pushP()
{
    setFlag(B, 1);

    bus_->writeCPU(0x0100 + sp_, p_);
    sp_--;
}

void NES_cpu::pullP()
{
    sp_++;
    p_ = bus_->readCPU(0x0100 + sp_);
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

// Debugging methods
void NES_cpu::printState()
{
    std::cout << "PC: " << std::hex << pc_ << ", SP: " << std::hex << (int)sp_ << ", A: " << std::hex << (int)A_
              << ", X: " << std::hex << (int)X_ << ", Y: " << std::hex << (int)Y_
              << ", NV1BDIZC: " << std::bitset<8>(p_) << ", Cycles: " << std::dec << cycle_ << std::endl;
}

// Error handling
void NES_cpu::handleInvalidOpcode()
{
    std::cerr << "Invalid opcode encountered at PC: " << std::hex << pc_ - 1 << std::endl;
}

uint16_t NES_cpu::handleInvalidAddressingMode()
{
    std::cerr << "Invalid addressing mode encountered at PC: " << std::hex << pc_ - 1 << std::endl;
    return 0;
}