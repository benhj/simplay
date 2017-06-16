// Copyright (c) 2017 Ben Jones

#pragma once

namespace neat {
    struct MutationParameters
    {
        // The rate at which a new node is added in place of
        // an existing connection. A--->B becomes A--C-->B
        double nodeAdditionProb;

        // The rate at which the type of a node changes
        double nodeFunctionChangeProb;

        // The rate at which weights are updated
        double weightChangeProb;

        // The rate at which a new conneciton is made from
        // an input node to a hidden node
        double connectionAdditionProb;
    };
}