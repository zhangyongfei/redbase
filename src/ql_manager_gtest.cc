#include "sm.h"
#include "catalog.h"
#include "gtest/gtest.h"
#include <sstream>

class QL_ManagerTest : public ::testing::Test {
};

TEST_F(QL_ManagerTest, Cons) {
    RC rc;
    PF_Manager pfm;
    IX_Manager ixm(pfm);
    RM_Manager rmm(pfm);
    SM_Manager smm(ixm, rmm);


    const char * dbname = "qlfile";
    stringstream comm;
    comm << "rm -rf " << dbname;
    rc = system (comm.str().c_str());

    stringstream command;
    command << "./dbcreate " << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);


    command.str("");
    command << "echo \"create table soaps(soapid  i, sname  c28, network  c4, rating  f);\" | ./redbase " 
            << dbname;
    cerr << command.str();

    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"create index soaps(soapid);\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    // wrong number of attrs
    command.str("");
    command << "echo \"insert into soaps values(\\\"The Good Wife\\\", \\\"CBS\\\", 4.0);\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_NE(rc, 0);

    command.str("");
    command << "echo \"insert into soaps values(133, \\\"The Good Wife\\\", \\\"CBS\\\", 4.0);\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    // float 4 vs int 4 - error
    command.str("");
    command << "echo \"insert into soaps values(133, \\\"The Good Wife\\\", \\\"CBS\\\", 4);\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_NE(rc, 0);

    command.str("");
    command << "echo \"load soaps(\\\"../soaps.data\\\");\" | ./redbase " 
            << dbname;
    cerr << command.str();
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"queryplans on;delete from soaps where soapid = 133;\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);
    
    command.str("");
    command << "echo \"queryplans on;delete from soaps where network = \\\"CBS\\\" and rating > 1.0;\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"queryplans on;delete from soaps where soapid > 3 and rating > 1.0;\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"queryplans on;delete from soaps;\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);

    command.str("");
    command << "echo \"queryplans on;delete from soaps where sname = \\\"The Good Wife\\\";\" | ./redbase " 
            << dbname;
    rc = system (command.str().c_str());
    ASSERT_EQ(rc, 0);


    rc = smm.OpenDb(dbname);
    ASSERT_EQ(rc, 0);

    rc = smm.Print("soaps");
    ASSERT_EQ(rc, 0);
    
    rc = smm.Print("relcat");
    ASSERT_EQ(rc, 0);

    rc = smm.CloseDb();
    ASSERT_EQ(rc, 0);

    stringstream command2;
    command2 << "./dbdestroy " << dbname;
    rc = system (command2.str().c_str());
    ASSERT_EQ(rc, 0);
}