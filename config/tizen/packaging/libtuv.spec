Name: libtuv
Version: 1.0.0
Release: 0
Summary: Asynchronous I/O for embedded system
Group: Development/System
License: Apache-2.0
Source:     %{name}-%{version}.tar.gz
Source1:    %{name}.pc.in
Source1001: %{name}.manifest
ExclusiveArch: %arm %ix86 x86_64

BuildRequires: cmake

%description
Asynchronous I/O for embedded system

# Initialize the variables
%{!?build_mode: %define build_mode release}
%{!?board: %define board None}
%{!?platform: %define platform noarch-tizen}

%package devel
Summary: Header files for %{name}
Group: Development/System
Requires: %{name} = %{version}-%{release}

%description devel
Development libraries for %{name}

%prep
%setup -q -c
cp %{SOURCE1001} .

%build
TUV_PLATFORM=%{platform} TUV_BOARD=%{board} TUV_BUILD_TYPE=%{build_mode} \
TUV_BUILDTESTER=no TUV_CREATE_SHARED_LIB=yes make

%install
mkdir -p %{buildroot}%{_includedir}/libtuv
mkdir -p %{buildroot}%{_libdir}/pkgconfig

cp ./build/%{platform}/%{build_mode}/lib/* %{buildroot}%{_libdir}/

cp ./include/*.h %{buildroot}%{_includedir}/libtuv
cp ./config/tizen/packaging/%{name}.pc.in %{buildroot}/%{_libdir}/pkgconfig/%{name}.pc

%post -p /sbin/ldconfig
%postun -p /sbin/ldconfig

%files
%manifest config/tizen/packaging/%{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libtuv.so
%license LICENSE

%files devel
%manifest config/tizen/packaging/%{name}.manifest
%defattr(-,root,root,-)
%{_libdir}/libtuv.*
%{_libdir}/pkgconfig/%{name}.pc
%{_includedir}/*
