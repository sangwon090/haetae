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

inline std::ostream& operator<<(std::ostream& os, const Operator& op) {
    switch (op) {
        case Operator::PipelineHost:
            os << "PipelineHost";
            break;
        case Operator::PipelineDevice:
            os << "PipelineDevice";
            break;
        case Operator::UnaryPlus:
            os << "UnaryPlus";
            break;
        case Operator::UnaryMinus:
            os << "UnaryMinus";
            break;
        case Operator::BitwiseAnd:
            os << "BitwiseAnd";
            break;
        case Operator::BitwiseOr:
            os << "BitwiseOr";
            break;
        case Operator::BitwiseXor:
            os << "BitwiseXor";
            break;
        case Operator::BitwiseNot:
            os << "BitwiseNot";
            break;
        case Operator::Plus:
            os << "Plus";
            break;
        case Operator::Minus:
            os << "Minus";
            break;
        case Operator::Multiply:
            os << "Multiply";
            break;
        case Operator::Divide:
            os << "Divide";
            break;
        case Operator::Modulo:
            os << "Modulo";
            break;
        case Operator::Equal:
            os << "Equal"; break;
        case Operator::NotEqual:
            os << "NotEqual"; break;
        case Operator::Less:
            os << "Less"; break;
        case Operator::LessEqual:
            os << "LessEqual"; break;
        case Operator::Greater:
            os << "Greater"; break;
        case Operator::GreaterEqual:
            os << "GreaterEqual"; break;
        case Operator::FnCall:
            os << "FnCall";
            break;
        default:
            os << "Unknown";
            break;
    }

    return os;
}