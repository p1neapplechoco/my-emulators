#pragma once

#include <cstdint>
#include <variant>

class NES_bus; // forward declaration: bus.h includes cpu.h

// CPU CONSTANTS
class NES_cpu
{
public:
    // MISCs
    void setBus(NES_bus &);

    // Flag based operations
    enum Flags : uint8_t
    {
        C = 1 << 0, // Carry
        Z = 1 << 1, // Zero
        I = 1 << 2, // Interrupt Disable
        D = 1 << 3, // Decimal

        B = 1 << 4, // no CPU effect, the B flag
        U = 1 << 5, // no CPU effect, always set as 1
        V = 1 << 6, // Overflow
        N = 1 << 7, // Negative
    };

    bool getFlag(Flags);
    void setFlag(Flags, bool);

    void initialize();
    void emulateCycle();
    void reset();

    // Instruction set
    using noArgInstr = void (NES_cpu::*)();
    using byteInstr = void (NES_cpu::*)(uint8_t);
    using addrInstr = void (NES_cpu::*)(uint16_t);

    using instr = std::variant<noArgInstr, byteInstr, addrInstr>;
    using addrMode = uint16_t (NES_cpu::*)();

    struct Instruction
    {
        instr operation;
        addrMode addressingMode;
        uint8_t cycles;
    };

    static Instruction instructionSet[256];

    // Addressing modes
    //// Indexed based
    uint16_t addrZeroPageX();
    uint16_t addrZeroPageY();
    uint16_t addrAbsoluteX();
    uint16_t addrAbsoluteY();
    uint16_t addrIndirectX();
    uint16_t addrIndirectY();

    //// Others
    uint16_t addrImplicit();
    uint16_t addrAccumulator();
    uint16_t addrImmediate();
    uint16_t addrZeroPage();
    uint16_t addrAbsolute();
    uint16_t addrRelative();
    uint16_t addrIndirect();

    // Access based
    void loadA(uint8_t);   // LDA
    void storeA(uint16_t); // STA
    void loadX(uint8_t);   // LDX
    void storeX(uint16_t); // STX
    void loadY(uint8_t);   // LDY
    void storeY(uint16_t); // STY

    // Transfer based
    void transferAtoX(); // TAX
    void transferXtoA(); // TXA
    void transferAtoY(); // TAY
    void transferYtoA(); // TYA

    // Arithmetic based
    void addWithCarry(uint8_t);      // ADC
    void subtractWithCarry(uint8_t); // SBC
    void incrementMemory(uint16_t);  // INC
    void decrementMemory(uint16_t);  // DEC
    void incrementX();               // INX
    void decrementX();               // DEX
    void incrementY();               // INY
    void decrementY();               // DEY

    // Shift based
    void shiftLeft(uint16_t);   // ASL
    void shiftRight(uint16_t);  // LSR
    void rotateLeft(uint16_t);  // ROL
    void rotateRight(uint16_t); // ROR

    // Bitwise based
    void bitwiseAnd(uint8_t); // AND
    void bitwiseOr(uint8_t);  // ORA
    void bitwiseXor(uint8_t); // EOR
    void bitTest(uint8_t);    // BIT

    // Compare based
    void compareA(uint8_t); // CPA
    void compareX(uint8_t); // CPX
    void compareY(uint8_t); // CPY

    // Branch based
    void branchIfCarryClear(uint16_t);    // BCC
    void branchIfCarrySet(uint16_t);      // BCS
    void branchIfEqual(uint16_t);         // BEQ
    void branchIfNotEqual(uint16_t);      // BNE
    void branchIfPlus(uint16_t);          // BPL
    void branchIfMinus(uint16_t);         // BMI
    void branchIfOverflowClear(uint16_t); // BVC
    void branchIfOverflowSet(uint16_t);   // BVS

    // Jump based
    void jumpTo(uint8_t);           // JMP
    void jumpToSubroutine(uint8_t); // JSR
    void returnFromSubroutine();    // RTS
    void interruptSoftware();       // BRK
    void returnFromInterrupt();     // RTI

    // Stack based
    void pushA();         // PHA
    void pullA();         // PLA
    void pushP();         // PHP
    void pullP();         // PLP
    void transferXtoSP(); // TXS
    void transferSPtoX(); // TSX

    // Flags based
    void clearC(); // CLC
    void setC();   // SEC
    void clearI(); // CLI
    void setI();   // SEI
    void clearD(); // CLD
    void setD();   // SED
    void clearV(); // CLV

    // NOP
    void noOperation(); // NOP

    // Debugging methods
    void printState();

    // Error handling
    void handleInvalidOpcode();
    uint16_t handleInvalidAddressingMode();

private:
    // Registers
    uint8_t A_; // Accumulator <- idek what this does
    uint8_t X_, Y_;
    uint16_t pc_;
    uint8_t sp_;
    uint8_t p_; // Status flags

    uint64_t cycle_;

    NES_bus *bus_;
};
