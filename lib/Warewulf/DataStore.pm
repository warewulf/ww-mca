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
use Warewulf::Message;
use Warewulf::Util;

our @ISA = ();

=head1 NAME

Warewulf::DataStore - Warewulf's data storage class

=head1 SYNOPSIS

    use Warewulf::DataStore;

    my $ds = Warewulf::DataStore->new();

    $ds->open($type);

    my $ObjectSet = $ds->get_objects();

    $ds->persist($ObjectSet);

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
cleared.  Any initializer accepted by the C<set()> method may also be
passed to C<init()>.

=cut

sub
init(@)
{
    my $self = shift;

    # Clear current data from object.
    %{$self} = ();

    # Check for new initializer.
    if (scalar(@_)) {
        $self->set(@_);
    }

    return $self;
}

=item open(I<type>)

Open the datastore by a given type (e.g. node, files, vnfs, etc.)

=cut

sub
open($)
{
    my ($self, $type) = @_;

}


=back

=head1 SEE ALSO

Warewulf::Object

=cut

1;
