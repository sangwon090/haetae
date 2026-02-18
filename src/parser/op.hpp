#pragma once

enum class Operator {
    PipelineHost,
    PipelineDevice,

    UnaryPlus,
    UnaryMinus,
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseNot,

    Plus,
    Minus,
    Multiply,
    Divide,
    Modulo,

    Equal,
    NotEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

    FnCall,
};