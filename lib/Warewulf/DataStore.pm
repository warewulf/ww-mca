#
# Copyright (c) 2001-2016, Gregory M. Kurtzer. All rights reserved.
# 
# "Warewulf" Copyright (c) 2003-2016, The Regents of the University of California,
# through Lawrence Berkeley National Laboratory (subject to receipt of any
# required approvals from the U.S. Dept. of Energy).  All rights reserved.
# 
# If you have questions about your rights to use or distribute this software,
# please contact Berkeley Lab's Innovation & Partnerships Office at
# IPO@lbl.gov.
# 
# NOTICE.  This Software was developed under funding from the U.S. Department of
# Energy and the U.S. Government consequently retains certain rights. As such,
# the U.S. Government has been granted for itself and others acting on its
# behalf a paid-up, nonexclusive, irrevocable, worldwide license in the Software
# to reproduce, distribute copies to the public, prepare derivative works, and
# perform publicly and display publicly, and to permit other to do so. 
# 



package Warewulf::DataStore;

use JSON;
use Data::Dumper;
use Warewulf::Object;
use Warewulf::ObjectSet;
#use Warewulf::Message;
#use Warewulf::Util;

our @ISA = ('Warewulf::Object');

=head1 NAME

Warewulf::DataStore - Warewulf's data storage class

=head1 SYNOPSIS

    use Warewulf::DataStore;

    my $ds = Warewulf::DataStore->new();
    $ds->open("/tmp/wwtest.json");

    my %hash = $ds->get_hash();

    if ( my $object = $ds->get_object($id) ) {
        $object->set("NAME", "test01");

        $ds->put_object($object);
    }

    my $o = Warewulf::Object->new();
    $o->set("FOO", "BAR");
    $o->set("NAME", "Test Object");

    $ds->put_object($o);

    $ds->persist("/tmp/wwtest.json");


=head1 DESCRIPTION

C<Warewulf::DataStore> Is the storage vector for Warewulf ObjectSets of
a given "type".

=head1 METHODS

=over 4

=item new()

Instantiate a datastore object. 

=cut

sub
new($)
{
    my $proto = shift;
    my $class = ref($proto) || $proto;
    my $self = {};

    bless($self, $class);

    return $self->init(@_);
}

=item init(...)

Initialize an object.  All data currently stored in the object will be
cleared. 

=cut

sub
init(@)
{
    my $self = shift;

    # Clear current data from object.
    %{$self} = ();

    $self->set("JSON", JSON->new->utf8(1)->pretty(1));

    return $self;
}

=item open()

Open a JSON datastore file

=cut

sub
open($)
{
    my ($self, $json_file) = @_;
    my $json = $self->get("JSON");

    $self->set("FILE", $json_file);

    if ( -f "$json_file" ) {
        my $fd;
        if ( ! open($fd, $json_file) ) {
            printf("ERROR: Could not open $json_file\n");
            return(undef);
        }
        $self->{"DATA"} = $json->decode(join("", <$fd>));

        close($fd);
    } else {
        $self->{"DATA"}{"WAREWULF"} = {};
    }

    return(0);
}



=item persist()

Persist/update the datastore

=cut

sub
persist($)
{
    my ($self, $json_file) = @_;
    my $json = $self->get("JSON");
    my $fd;

    if ( ! $json_file ) {
        $json_file = $self->get("FILE");
    }

    if ( ! open($fd, ">". $json_file) ) {
        printf("ERROR: Could not open $json_file\n");
        return(undef);
    }

    print $fd $json->encode($self->{"DATA"});

    close($fd);

}



=item get_hash()

Return the complete perl hash of data

=cut

sub
get_hash($)
{
    my ($self) = @_;
    my $scalar;

    if ( exists($self->{"DATA"}) ) {
        return(%{$self->{"DATA"}{"WAREWULF"}});
    } else {
        printf("WARNING: No cached DATA!\n");
    }

    return(undef);
}



=item get_object()

Return an Object for a particular 

=cut

sub
get_objects(@)
{
    my ($self, @ids) = @_;
    my $ObjectSet = Warewulf::ObjectSet->new();
    my $count = 0;
 
    if ( exists($self->{"DATA"}) ) {
        foreach my $id ( @ids ) {
            if ( exists($self->{"DATA"}{"WAREWULF"}{"OBJECTS"}{$id})) {
                my $o = Warewulf::Object->new($self->{"DATA"}{"WAREWULF"}{"OBJECTS"}{$id});
                $o->set("ID", $id);
                $ObjectSet->add($o);
                $count++;
            }
        }
    }
    if ( $count > 0 ) {
        return($ObjectSet);
    }
    return(undef);
}


=item put_object()

Push objects into JSON datastore

=cut

sub
put_objects($$)
{
    my ($self, $o) = @_;
    my $count = 0;

    if ( ref($o) eq "Warewulf::Object" ) {
        my $id = $o->get("ID");

        if ( ! $id ) {
            #TODO: Modularize this, and do this better
            $id = `cat /proc/sys/kernel/random/uuid`;
            chomp($id);
        }

        if ( exists($self->{"DATA"}) ) {
            $self->{"DATA"}{"WAREWULF"}{"OBJECTS"}{"$id"} = $o->get_hash();
            $count++;
        }

    } elsif ( ref($o) eq "Warewulf::Object" ) {
        for my $o ( $o->get_list() ) {
            my $id = $object->get("ID");

            if ( ! $id ) {
                #TODO: Modularize this, and do this better
                $id = `cat /proc/sys/kernel/random/uuid`;
                chomp($id);
            }

            if ( exists($self->{"DATA"}) ) {
                $self->{"DATA"}{"WAREWULF"}{"OBJECTS"}{"$id"} = $object->get_hash();
                $count++;
            }
        }
    }

    return($count);
}


=item find()

Find objects based on key/value and return their IDs

=cut

sub
find($$)
{
    my ($self, $key, $value) = @_;
    my @ids;
    my $count = 0;

    if ( exists($self->{"DATA"}) ) {
        foreach my $id ( keys %{$self->{"DATA"}{"WAREWULF"}{"OBJECTS"}} ) {
            if ( exists($self->{"DATA"}{"WAREWULF"}{"OBJECTS"}{"$id"}{"$key"}) ) {
                if ( $self->{"DATA"}{"WAREWULF"}{"OBJECTS"}{"$id"}{"$key"} eq $value ) {
                    push(@ids, $id);
                }
            }
        }
    }

    return(@ids);
}



=back

=head1 SEE ALSO

Warewulf::Object Warewulf::ObjectSet

=cut



    my $ds = Warewulf::DataStore->new();
    $ds->open("/tmp/wwtest.json");

    my @ids = $ds->find("FOO", "BAR");
    
    if ( my $ObjectSet = $ds->get_objects(@ids) ) {
    printf("Updating objectset\n");
        $ObjectSet->set("NAME", "test01");

        $ds->put_objects($ObjectSet);
    }

    my $o = Warewulf::Object->new();
    $o->set("FOO", "BAR");
    $o->set("NAME", "Test Object");

    $ds->put_objects($o);

    $ds->persist("/tmp/wwtest.json");




1;
