#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Client connects to Server and can close both connections" {
    
    run ./dsh -s &

    sleep 1

    run ./dsh -c <<EOF
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:clientrequestedservertostop,stopping...cmdloopreturned-7"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
     # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

}

@test "Server and Client can use -i and -p to choose address and port number" {
    
    run ./dsh -s -i 0.0.0.0 -p 7890 &

    sleep 1

    run ./dsh -c -i 0.0.0.0 -p 7890 <<EOF
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:0.0.0.0:7890Entercommand:clientrequestedservertostop,stopping...cmdloopreturned-7"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
     # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

}


@test "Error messages still allow client to continue typing commands" {
    
    run ./dsh -s &

    sleep 1

    run ./dsh -c <<EOF
echo 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
| echo
noCommand
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:error:pipinglimitedto8commandsEntercommand:CommandcannotstartorendwithpipesEntercommand:socketservermode:addr:0.0.0.0:1234->Single-ThreadedModeServerislisteningon0.0.0.0:1234CommandisinvalidornotimplementedEntercommand:clientrequestedservertostop,stopping...cmdloopreturned-7"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

}

@test "Handles echo strings properly" {
    
    run ./dsh -s &

    sleep 1

    run ./dsh -c <<EOF
echo "         hey"
echo "fail
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v[:cntrl:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="socket client mode:  addr:127.0.0.1:1234Enter command:          heyEnter command: Error: Mismatched quotesEnter command: client requested server to stop, stopping...cmd loop returned -7"
    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

}

@test "General Error Messages will show for on Client Interface" {
    
    run ./dsh -s &

    sleep 1

    run ./dsh -c <<EOF

stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\t\n\r\f\v[:cntrl:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="socket client mode:  addr:127.0.0.1:1234Enter command: warning: no commands providedEnter command: client requested server to stop, stopping...cmd loop returned -7"
    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

}

@test "dsh command with -x will not work without -s" {
    
    run ./dsh -x

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="Error:-xcanonlybeusedwith-s"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

}

@test "dsh command with -h will show help" {
    
    run ./dsh -h

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="Usage:./dsh[-c|-s][-iIP][-pPORT][-x][-h]Defaultistorun./dshinlocalmode-cRunasclient-sRunasserver-iIPSetIP/Interfaceaddress(onlyvalidwith-cor-s)-pPORTSetportnumber(onlyvalidwith-cor-s)-xEnablethreadedmode(onlyvalidwith-s)-hShowthishelpmessage"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

}

@test "dsh command with -p will not work without -c or -s" {
    
    run ./dsh -p

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="./dsh:optionrequiresanargument--'p'Usage:./dsh[-c|-s][-iIP][-pPORT][-x][-h]Defaultistorun./dshinlocalmode-cRunasclient-sRunasserver-iIPSetIP/Interfaceaddress(onlyvalidwith-cor-s)-pPORTSetportnumber(onlyvalidwith-cor-s)-xEnablethreadedmode(onlyvalidwith-s)-hShowthishelpmessage"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

}

@test "dsh command with -i will not work without -c or -s" {
    
    run ./dsh -i

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="./dsh:optionrequiresanargument--'i'Usage:./dsh[-c|-s][-iIP][-pPORT][-x][-h]Defaultistorun./dshinlocalmode-cRunasclient-sRunasserver-iIPSetIP/Interfaceaddress(onlyvalidwith-cor-s)-pPORTSetportnumber(onlyvalidwith-cor-s)-xEnablethreadedmode(onlyvalidwith-s)-hShowthishelpmessage"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

}



@test "Client cannot connect to invalid or unopen server" {
    
    run ./dsh -c <<EOF
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Attemptingtoconnectto127.0.0.1:1234cmdloopreturned-52"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

     # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

}

@test "Client can exit Server without closing it, which allows other clients to join" {
    
    run ./dsh -s &
    sleep 1
    run ./dsh -c <<EOF 
exit
EOF
    echo "$output" > tmpfile
    run ./dsh -c <<EOF
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(cat tmpfile | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:clientexited:gettingnextconnection...cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
    # Assertions
    [ "$status" -eq 0 ]
    
    rm tmpfile

}

@test "Normal Single Commands work properly in client" {
    
    run ./dsh -s &
    sleep 1
    run ./dsh -c <<EOF 
echo hi
EOF
    echo "$output" > tmpfile
    run ./dsh -c <<EOF
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(cat tmpfile | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:hiEntercommand:cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
    # Assertions
    [ "$status" -eq 0 ]
    
    rm tmpfile

}

@test "Commands with multiple pipes work properly in client" {
    
    run ./dsh -s &
    sleep 1
    run ./dsh -c <<EOF 
echo hello | grep hello | wc -l
EOF
    echo "$output" > tmpfile
    run ./dsh -c <<EOF
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(cat tmpfile | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:1Entercommand:cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
    # Assertions
    [ "$status" -eq 0 ]
    
    rm tmpfile

}

@test "More pipe command checks" {
    
    run ./dsh -s &
    sleep 1
    run ./dsh -c <<EOF 
echo hi | bruh | echo hey
EOF
    echo "$output" > tmpfile
    run ./dsh -c <<EOF
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(cat tmpfile | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:heyEntercommand:cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
    # Assertions
    [ "$status" -eq 0 ]
    
    rm tmpfile

}

@test "cd command works inside client" {
    
    run ./dsh -s &
    sleep 1
    
    run ./dsh -c <<EOF 
mkdir test-dir
cd test-dir
pwd
EOF
    echo "$output" > tmpfile
    run ./dsh -c <<EOF
stop-server
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(cat tmpfile | tr -d '\r[:cntrl:][:space:]')
    

    # Expected output with all whitespace removed for easier matching
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:Entercommand:Entercommand:$(pwd)/test-dirEntercommand:cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $stripped_expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"
    # Check exact match
    [ "$stripped_output" = "$expected_output" ]
    # Assertions
    [ "$status" -eq 0 ]
    
    rm tmpfile
    rmdir test-dir

}
@test "Client-Server still works even in different directories" {
    run ./dsh -s &
    sleep 1

    mkdir test-dir1

    cp dsh test-dir1

    cd test-dir1

    run ./dsh -c <<EOF
stop-server
EOF

    stripped_output=$(echo "$output" | tr -d '\r[:cntrl:][:space:]')

    # Define the expected output (strip all whitespace)
    expected_output="socketclientmode:addr:127.0.0.1:1234Entercommand:clientrequestedservertostop,stopping...cmdloopreturned-7"  # Update this with the expected output

    # Debugging: print the captured and expected output for easier comparison
    echo "Captured stdout:"
    echo "Output: $stripped_output"
    echo "Expected Output: $expected_output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Assertions: Check if the stripped output matches the expected output
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

    cd ..
    rm -r test-dir1
}


@test "dsh works normally without any arguments" {
    run "./dsh" <<EOF                
exit
EOF

    # Strip all whitespace (spaces, tabs, newlines) from the output
    stripped_output=$(echo "$output" | tr -d '[:space:]')

    # Expected output with all whitespace removed for easier matching
    expected_output="localmodedsh4>"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]
}
