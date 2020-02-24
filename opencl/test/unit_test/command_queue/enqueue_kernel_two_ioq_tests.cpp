/*
 * Copyright (C) 2017-2020 Intel Corporation
 *
 * SPDX-License-Identifier: MIT
 *
 */

#include "opencl/source/command_queue/command_queue.h"
#include "opencl/source/event/event.h"
#include "opencl/test/unit_test/fixtures/hello_world_fixture.h"
#include "opencl/test/unit_test/helpers/hw_parse.h"
#include "test.h"

using namespace NEO;

struct TwoIOQsTwoDependentWalkers : public HelloWorldTest<HelloWorldFixtureFactory>,
                                    public HardwareParse {
    typedef HelloWorldTest<HelloWorldFixtureFactory> Parent;
    using Parent::createCommandQueue;
    using Parent::pCmdQ;
    using Parent::pDevice;
    using Parent::pKernel;

    TwoIOQsTwoDependentWalkers() {
    }

    void SetUp() override {
        Parent::SetUp();
        HardwareParse::SetUp();
    }

    void TearDown() override {
        delete pCmdQ2;
        HardwareParse::TearDown();
        Parent::TearDown();
    }

    template <typename FamilyType>
    void parseWalkers() {
        cl_uint workDim = 1;
        size_t globalWorkOffset[3] = {0, 0, 0};
        size_t globalWorkSize[3] = {1, 1, 1};
        size_t localWorkSize[3] = {1, 1, 1};
        cl_event event1 = nullptr;
        cl_event event2 = nullptr;

        auto retVal = pCmdQ->enqueueKernel(
            pKernel,
            workDim,
            globalWorkOffset,
            globalWorkSize,
            localWorkSize,
            0,
            nullptr,
            &event1);

        ASSERT_EQ(CL_SUCCESS, retVal);
        HardwareParse::parseCommands<FamilyType>(*pCmdQ);

        // Create a second command queue (beyond the default one)
        pCmdQ2 = createCommandQueue(pClDevice);
        ASSERT_NE(nullptr, pCmdQ2);

        retVal = pCmdQ2->enqueueKernel(
            pKernel,
            workDim,
            globalWorkOffset,
            globalWorkSize,
            localWorkSize,
            1,
            &event1,
            &event2);

        ASSERT_EQ(CL_SUCCESS, retVal);
        HardwareParse::parseCommands<FamilyType>(*pCmdQ2);

        Event *E1 = castToObject<Event>(event1);
        ASSERT_NE(nullptr, E1);
        Event *E2 = castToObject<Event>(event2);
        ASSERT_NE(nullptr, E2);
        delete E1;
        delete E2;

        typedef typename FamilyType::WALKER_TYPE GPGPU_WALKER;
        itorWalker1 = find<GPGPU_WALKER *>(cmdList.begin(), cmdList.end());
        ASSERT_NE(cmdList.end(), itorWalker1);

        itorWalker2 = itorWalker1;
        ++itorWalker2;
        itorWalker2 = find<GPGPU_WALKER *>(itorWalker2, cmdList.end());
        ASSERT_NE(cmdList.end(), itorWalker2);
    }

    GenCmdList::iterator itorWalker1;
    GenCmdList::iterator itorWalker2;
    CommandQueue *pCmdQ2 = nullptr;
};

HWTEST_F(TwoIOQsTwoDependentWalkers, GivenTwoCommandQueuesWhenEnqueuingKernelThenTwoDifferentWalkersAreCreated) {
    parseWalkers<FamilyType>();
    EXPECT_NE(itorWalker1, itorWalker2);
}

HWTEST_F(TwoIOQsTwoDependentWalkers, GivenTwoCommandQueuesWhenEnqueuingKernelThenOnePipelineSelectExists) {
    parseWalkers<FamilyType>();
    int numCommands = getNumberOfPipelineSelectsThatEnablePipelineSelect<FamilyType>();
    EXPECT_EQ(1, numCommands);
}

HWCMDTEST_F(IGFX_GEN8_CORE, TwoIOQsTwoDependentWalkers, GivenTwoCommandQueuesWhenEnqueuingKernelThenThereIsOneVfeState) {
    parseWalkers<FamilyType>();

    auto numCommands = getCommandsList<typename FamilyType::MEDIA_VFE_STATE>().size();
    EXPECT_EQ(1u, numCommands);
}

HWTEST_F(TwoIOQsTwoDependentWalkers, GivenTwoCommandQueuesWhenEnqueuingKernelThenOnePipeControlIsInsertedBetweenWalkers) {
    typedef typename FamilyType::PIPE_CONTROL PIPE_CONTROL;

    parseWalkers<FamilyType>();
    auto itorCmd = find<PIPE_CONTROL *>(itorWalker1, itorWalker2);

    // Should find a PC.
    EXPECT_NE(itorWalker2, itorCmd);
}