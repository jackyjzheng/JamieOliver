import boto3

ec2 = boto3.resource('ec2', region_name="us-east-1")

instance = ec2.create_instances(
    ImageId='ami-0df45c7a1da7d88a8',
    MinCount=1,
    MaxCount=1,
    InstanceType='t2.micro')